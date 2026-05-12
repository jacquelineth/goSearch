// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "UILauncher.hpp"
#include "FramePersistenceHandler.hpp"
#include "SearchListener.hpp"
#include "SortedPersistedAutoCompleter.hpp"
#include "Theme.hpp"
#include "results/ListResultRenderer.hpp"
#include "results/TableResultRenderer.hpp"
#include "../util/Configuration.hpp"
#include "../util/PreferencesManager.hpp"

#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QVBoxLayout>

namespace javatb {

UILauncher::UILauncher(const QString& rootPath, QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Find4j");

    // Select result renderer from config
    auto rendererClass = Configuration::instance().getString(ConfigurationProperty::RESULT_RENDERER);
    if (rendererClass == "ListResultRenderer")
        resultRenderer_ = new ListResultRenderer(this);
    else
        resultRenderer_ = new TableResultRenderer(this);

    if (rootPath.isEmpty()) {
        pathField_ = new QLineEdit(QDir::currentPath());
    } else {
        pathField_ = new QLineEdit(rootPath);
    }

    setCentralWidget(createCentralWidget());
    resize(900, 600);

    framePersistence_ = std::make_unique<FramePersistenceHandler>("main.frame", this);
}

UILauncher::~UILauncher() {
    if (searchThread_.joinable()) {
        if (engine_) engine_->setStopped(true);
        searchThread_.join();
    }
}

// ── Layout ────────────────────────────────────────────────────────────────────

QWidget* UILauncher::createCentralWidget() {
    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(createTopPanel());
    splitter->addWidget(resultRenderer_->getWidget());
    splitter->addWidget(createStatusBar());
    splitter->setStretchFactor(1, 1); // results get all extra space
    splitter->setHandleWidth(1);
    return splitter;
}

QWidget* UILauncher::createTopPanel() {
    auto* container = new QWidget();
    auto* layout    = new QVBoxLayout(container);
    layout->setContentsMargins(5, 5, 5, 2);
    layout->setSpacing(4);

    // Row 1: path selector
    {
        auto* row  = new QHBoxLayout();
        auto* lbl  = new QLabel("Search in");
        auto* btn  = new QPushButton("...");
        btn->setFixedWidth(30);
        btn->setToolTip("Select a folder or archive as the search root");
        connect(btn, &QPushButton::clicked, this, &UILauncher::chooseFolder);
        row->addWidget(lbl);
        row->addWidget(pathField_, 1);
        row->addWidget(btn);
        layout->addLayout(row);
    }

    // Row 2: file pattern + search term + Go/Stop
    {
        fileField_  = new QLineEdit();
        fileField_->setToolTip("File pattern (* and ? wildcards accepted)");
        fileFieldCompleter_ = new SortedPersistedAutoCompleter(fileField_, "filePattern", this);

        termField_  = new QLineEdit();
        termField_->setToolTip("Text to search for inside matched files");
        termFieldCompleter_ = new SortedPersistedAutoCompleter(termField_, "searchTerm", this);

        goButton_   = new QPushButton("Go");
        goButton_->setFixedWidth(60);
        goButton_->setDefault(true);
        goButton_->setToolTip("Launch search");

        stopButton_ = new QPushButton("Stop");
        stopButton_->setFixedWidth(70);
        stopButton_->setEnabled(false);
        stopButton_->setToolTip("Stop current search");

        connect(goButton_,   &QPushButton::clicked, this, &UILauncher::startSearch);
        connect(stopButton_, &QPushButton::clicked, this, &UILauncher::stopSearch);

        auto* row = new QHBoxLayout();
        row->addWidget(new QLabel("File pattern"));
        row->addWidget(fileField_, 1);
        row->addWidget(new QLabel("Search term"));
        row->addWidget(termField_, 1);
        row->addWidget(goButton_);
        row->addWidget(stopButton_);
        layout->addLayout(row);
    }

    return container;
}

QWidget* UILauncher::createStatusBar() {
    auto* container = new QWidget();
    auto* row = new QHBoxLayout(container);
    row->setContentsMargins(5, 2, 5, 2);

    exploringField_ = new QLineEdit();
    exploringField_->setReadOnly(true);
    exploringField_->setFrame(false);

    nbMatchesLabel_ = new QLabel("0");
    nbMatchesLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    nbMatchesLabel_->setMinimumWidth(50);
    nbMatchesLabel_->setMaximumWidth(50);

    row->addWidget(new QLabel("Exploring"));
    row->addWidget(exploringField_, 1);
    row->addWidget(new QLabel("Matches"));
    row->addWidget(nbMatchesLabel_);

    return container;
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void UILauncher::startSearch() {
    if (searching_.load()) return;

    fileFieldCompleter_->updateListData();
    termFieldCompleter_->updateListData();
    resultRenderer_->clear();
    nbMatches_ = 0;
    nbMatchesLabel_->setText("0");
    exploringField_->setText({});

    goButton_->setEnabled(false);
    stopButton_->setEnabled(true);

    std::vector<std::string> args {
        pathField_->text().toStdString(),
        fileField_->text().toStdString(),
        termField_->text().toStdString(),
    };

    engine_ = std::make_unique<SearchEngine>(std::move(args));
    listener_ = std::make_unique<SearchListener>(this, this);
    engine_->addSearchEngineListener(listener_.get());

    searching_.store(true);
    searchThread_ = std::thread([this]() {
        try {
            engine_->search();
        } catch (...) {}
        searching_.store(false);
        // Final UI reset is handled by onSearchEnded via signal
    });
}

void UILauncher::stopSearch() {
    if (engine_) engine_->setStopped(true);
}

void UILauncher::chooseFolder() {
    QString start = pathField_->text();
    QFileInfo fi(start);
    if (!fi.exists()) start = QDir::currentPath();
    else if (!fi.isDir()) start = fi.dir().absolutePath();

    QString chosen = QFileDialog::getExistingDirectory(this,
        "Select search root", start,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!chosen.isEmpty())
        pathField_->setText(chosen);
}

void UILauncher::onMatchFound(std::shared_ptr<SearchElement> element) {
    ++nbMatches_;
    nbMatchesLabel_->setText(QString::number(nbMatches_));
    resultRenderer_->addElement(std::move(element));
}

void UILauncher::onExploringPath(const QString& path) {
    exploringField_->setText(path);
}

void UILauncher::onSearchStarted() {
    nbMatches_ = 0;
    nbMatchesLabel_->setText("0");
}

void UILauncher::onSearchEnded(int matchCount) {
    goButton_->setEnabled(true);
    stopButton_->setEnabled(false);
    if (matchCount == 0) {
        exploringField_->setStyleSheet("color: red;");
        exploringField_->setText("No match found");
    } else {
        exploringField_->setStyleSheet({});
        exploringField_->setText({});
    }
    if (searchThread_.joinable()) searchThread_.detach();
    engine_.reset();
    listener_.reset();
}

void UILauncher::closeEvent(QCloseEvent* event) {
    if (searching_.load() && engine_)
        engine_->setStopped(true);
    PreferencesManager::saveAll();
    event->accept();
}

} // namespace javatb
