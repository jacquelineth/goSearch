package main

import (
	"fmt"
	"os"
	"path/filepath"
	"sync"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/widget"
	search "goSearch/internal/search"
)

func loadIcon(path string) fyne.Resource {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil
	}
	return fyne.NewStaticResource(filepath.Base(path), data)
}

func defaultRootPath() string {
	if len(os.Args) > 1 {
		return os.Args[1]
	}
	wd, err := os.Getwd()
	if err != nil {
		return "."
	}
	return wd
}

func main() {
	a := app.New()
	w := a.NewWindow("Find4j")
	if icon := loadIcon(filepath.Join("images", "JavaTB-icon.gif")); icon != nil {
		w.SetIcon(icon)
	}

	rootEntry := widget.NewEntry()
	rootEntry.SetText(defaultRootPath())
	fileEntry := widget.NewEntry()
	termEntry := widget.NewEntry()
	resultArea := widget.NewMultiLineEntry()
	resultArea.Wrapping = fyne.TextWrapWord
	resultArea.Disable()
	var resultsBuf string
	statusLabel := widget.NewLabel("Ready")
	exploringLabel := widget.NewLabel("")

	var engine *search.SearchEngine
	var engineMutex sync.Mutex

	chooseButton := widget.NewButton("...", func() {
		dialog.NewFolderOpen(func(uri fyne.ListableURI, err error) {
			if err != nil {
				dialog.ShowError(err, w)
				return
			}
			if uri != nil {
				rootEntry.SetText(uri.Path())
			}
		}, w).Show()
	})

	var goButton *widget.Button
	stopButton := widget.NewButton("Stop", func() {
		engineMutex.Lock()
		defer engineMutex.Unlock()
		if engine != nil {
			engine.SetStopped(true)
			statusLabel.SetText("Stopping...")
		}
	})
	stopButton.Disable()

	goButton = widget.NewButton("Go", func() {
		engineMutex.Lock()
		if engine != nil {
			engineMutex.Unlock()
			return
		}
		engineMutex.Unlock()

		resultArea.SetText("")
		statusLabel.SetText("Starting search...")
		exploringLabel.SetText("")
		goButton.Disable()
		stopButton.Enable()

		properties, err := search.LoadConfigProperties(filepath.Join("config", "config.properties"))
		if err != nil {
			properties = map[string]string{}
		}
		archiveExts := search.LoadZipExtensions(properties)

		engine = search.NewSearchEngine(rootEntry.Text, fileEntry.Text, termEntry.Text, archiveExts)

		go func() {
			defer func() {
				goButton.Enable()
				stopButton.Disable()
				engineMutex.Lock()
				engine = nil
				engineMutex.Unlock()
			}()

			err := engine.Search(func(event search.SearchEvent) {
				switch event.Type {
				case search.SearchEventTypeExploring:
					exploringLabel.SetText(fmt.Sprintf("Exploring %s", event.Element.String()))
					statusLabel.SetText("Searching...")
				case search.SearchEventTypeMatch:
					resultsBuf += event.Element.String() + "\n"
					resultArea.SetText(resultsBuf)
				case search.SearchEventTypeEnd:
					statusLabel.SetText("Search complete")
				}
			})
			if err != nil {
				dialog.ShowError(err, w)
				statusLabel.SetText("Search failed")
			}
		}()
	})

	searchPanel := container.NewGridWithColumns(3,
		widget.NewLabel("Search in"), rootEntry, chooseButton,
	)
	criteriaPanel := container.NewGridWithColumns(4,
		widget.NewLabel("File pattern"), fileEntry,
		widget.NewLabel("Search term"), termEntry,
	)
	buttonsPanel := container.NewHBox(goButton, stopButton)
	statusPanel := container.NewVBox(statusLabel, exploringLabel)

	content := container.NewBorder(
		container.NewVBox(searchPanel, criteriaPanel, buttonsPanel),
		statusPanel,
		nil,
		nil,
		resultArea,
	)

	w.SetContent(content)
	w.Resize(fyne.NewSize(800, 600))
	w.ShowAndRun()
}
