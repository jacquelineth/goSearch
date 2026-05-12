// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.UILauncher
#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>

#include "../search/SearchElement.hpp"
#include "../search/SearchEngine.hpp"
#include "results/ResultRenderer.hpp"

namespace javatb {

class SortedPersistedAutoCompleter;
class FramePersistenceHandler;
class SearchListener;

class UILauncher final : public QMainWindow {
    Q_OBJECT
public:
    explicit UILauncher(const QString& rootPath = {}, QWidget* parent = nullptr);
    ~UILauncher() override;

    [[nodiscard]] bool isSearching() const noexcept { return searching_.load(); }

public slots:
    void onMatchFound(std::shared_ptr<SearchElement> element);
    void onExploringPath(const QString& path);
    void onSearchStarted();
    void onSearchEnded(int matchCount);

private slots:
    void startSearch();
    void stopSearch();
    void chooseFolder();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QWidget* createCentralWidget();
    QWidget* createTopPanel();
    QWidget* createStatusBar();

    // ── Input fields ─────────────────────────────────────────────────────────
    QLineEdit* pathField_  = nullptr;
    QLineEdit* fileField_  = nullptr;
    QLineEdit* termField_  = nullptr;

    // ── Auto-completers ───────────────────────────────────────────────────────
    SortedPersistedAutoCompleter* fileFieldCompleter_ = nullptr;
    SortedPersistedAutoCompleter* termFieldCompleter_ = nullptr;

    // ── Status bar ────────────────────────────────────────────────────────────
    QLabel*    nbMatchesLabel_  = nullptr;
    QLineEdit* exploringField_  = nullptr;

    // ── Buttons ───────────────────────────────────────────────────────────────
    QPushButton* goButton_   = nullptr;
    QPushButton* stopButton_ = nullptr;

    // ── Result view ───────────────────────────────────────────────────────────
    ResultRenderer* resultRenderer_ = nullptr;

    // ── Search state ─────────────────────────────────────────────────────────
    std::unique_ptr<SearchEngine> engine_;
    std::unique_ptr<SearchListener> listener_;
    std::thread searchThread_;
    std::atomic<bool> searching_{false};
    int nbMatches_ = 0;

    // ── Persistence ───────────────────────────────────────────────────────────
    std::unique_ptr<FramePersistenceHandler> framePersistence_;
};

} // namespace javatb
