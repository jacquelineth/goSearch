package search

import (
	"archive/zip"
	"bytes"
	"io"
	"os"
	"path/filepath"
	"regexp"
	"strings"
)

type SearchListener func(event SearchEvent)

type SearchEventType int

const (
	SearchEventTypeStart SearchEventType = iota
	SearchEventTypeExploring
	SearchEventTypeMatch
	SearchEventTypeEnd
)

type SearchEvent struct {
	Type    SearchEventType
	Element SearchElement
}

type SearchEngine struct {
	rootPath       string
	filePattern    *regexp.Regexp
	contentPattern *regexp.Regexp
	currentPath    []SearchElement
	stopped        bool
	archiveExts    map[string]struct{}
}

func NewSearchEngine(rootPath, filePattern, searchTerm string, archiveExts map[string]struct{}) *SearchEngine {
	if strings.TrimSpace(rootPath) == "" {
		rootPath = "."
	}
	if strings.TrimSpace(filePattern) == "" {
		filePattern = "*"
	}
	pattern := patternToRegexp(filePattern)
	var content *regexp.Regexp
	if strings.TrimSpace(searchTerm) != "" {
		content = regexp.MustCompile(`(?i)` + regexp.QuoteMeta(searchTerm))
	}
	if archiveExts == nil {
		archiveExts = LoadZipExtensions(nil)
	}
	return &SearchEngine{
		rootPath:       rootPath,
		filePattern:    pattern,
		contentPattern: content,
		archiveExts:    archiveExts,
	}
}

func patternToRegexp(pattern string) *regexp.Regexp {
	var builder strings.Builder
	for _, c := range strings.ToLower(pattern) {
		switch c {
		case '*':
			builder.WriteString(".*")
		case '?':
			builder.WriteRune('.')
		case '.':
			builder.WriteString("\\.")
		default:
			builder.WriteRune(c)
		}
	}
	return regexp.MustCompile(builder.String())
}

func (se *SearchEngine) Search(listener SearchListener) error {
	if se == nil {
		return nil
	}
	if listener != nil {
		listener(SearchEvent{Type: SearchEventTypeStart, Element: SearchElement{Name: se.rootPath}})
	}
	paths := strings.Split(se.rootPath, string(os.PathListSeparator))
	for _, path := range paths {
		if err := se.searchPath(strings.TrimSpace(path), listener); err != nil {
			return err
		}
	}
	if listener != nil {
		listener(SearchEvent{Type: SearchEventTypeEnd, Element: SearchElement{Name: se.rootPath}})
	}
	return nil
}

func (se *SearchEngine) searchPath(path string, listener SearchListener) error {
	if se.IsStopped() || strings.TrimSpace(path) == "" {
		return nil
	}
	info, err := os.Stat(path)
	if err != nil {
		return nil
	}
	name := info.Name()
	match := se.filePattern.MatchString(strings.ToLower(name))
	if info.IsDir() {
		if se.contentPattern != nil {
			match = false
		}
		elt, err := NewSearchElementFromFile(path, FolderType)
		if err != nil {
			return nil
		}
		if listener != nil {
			listener(SearchEvent{Type: SearchEventTypeExploring, Element: elt})
		}
		entries, err := os.ReadDir(path)
		if err != nil {
			return err
		}
		for _, entry := range entries {
			if se.IsStopped() {
				return nil
			}
			childPath := filepath.Join(path, entry.Name())
			if err := se.searchPath(childPath, listener); err != nil {
				return err
			}
		}
		return nil
	}

	ext := strings.ToLower(FileExtension(name))
	if _, ok := se.archiveExts[ext]; ok {
		elt, err := NewSearchElementFromFile(path, ArchiveType|FileType)
		if err != nil {
			return nil
		}
		match = false
		if listener != nil {
			listener(SearchEvent{Type: SearchEventTypeExploring, Element: elt})
		}
		se.addElementToPath(elt)
		defer se.removeLastPathElement()
		data, err := os.ReadFile(path)
		if err == nil {
			se.searchArchive(data, listener)
		}
	} else {
		elt, err := NewSearchElementFromFile(path, FileType)
		if err != nil {
			return nil
		}
		if match && se.contentPattern != nil && CheckEnoughMemory(elt.Size) {
			content, err := ReadTextFile(path)
			if err == nil {
				match = match && se.contentPattern.MatchString(content)
			} else {
				match = false
			}
		}
		if match && listener != nil {
			listener(SearchEvent{Type: SearchEventTypeMatch, Element: elt})
		}
	}
	return nil
}

func (se *SearchEngine) searchArchive(data []byte, listener SearchListener) error {
	if se.IsStopped() {
		return nil
	}
	reader := bytes.NewReader(data)
	zr, err := zip.NewReader(reader, int64(len(data)))
	if err != nil {
		return nil
	}
	for _, entry := range zr.File {
		if se.IsStopped() {
			return nil
		}
		if entry.FileInfo().IsDir() {
			continue
		}
		name := filepath.Base(entry.Name)
		match := se.filePattern.MatchString(strings.ToLower(name))
		cpath := se.getCurrentPath()
		ext := strings.ToLower(FileExtension(entry.Name))
		if _, ok := se.archiveExts[ext]; ok {
			elt := NewSearchElementFromZipFile(entry, ArchiveType|EntryType, cpath)
			match = false
			se.addElementToPath(elt)
			if listener != nil {
				listener(SearchEvent{Type: SearchEventTypeExploring, Element: elt})
			}
			data, err := ZipEntryAsBytes(openZipEntry(entry))
			if err == nil {
				_ = se.searchArchive(data, listener)
			}
			se.removeLastPathElement()
			continue
		}
		elt := NewSearchElementFromZipFile(entry, EntryType, cpath)
		if match && se.contentPattern != nil && CheckEnoughMemory(elt.Size) {
			rc, err := entry.Open()
			if err == nil {
				content, err := ReadTextFileFromReader(rc)
				rc.Close()
				if err == nil {
					match = match && se.contentPattern.MatchString(content)
				} else {
					match = false
				}
			} else {
				match = false
			}
		}
		if match && listener != nil {
			listener(SearchEvent{Type: SearchEventTypeMatch, Element: elt})
		}
	}
	return nil
}

func openZipEntry(entry *zip.File) io.Reader {
	rc, err := entry.Open()
	if err != nil {
		return bytes.NewReader(nil)
	}
	data, err := ZipEntryAsBytes(rc)
	rc.Close()
	if err != nil {
		return bytes.NewReader(nil)
	}
	return bytes.NewReader(data)
}

func (se *SearchEngine) addElementToPath(elt SearchElement) {
	se.currentPath = append(se.currentPath, elt)
}

func (se *SearchEngine) removeLastPathElement() {
	if len(se.currentPath) > 0 {
		se.currentPath = se.currentPath[:len(se.currentPath)-1]
	}
}

func (se *SearchEngine) getCurrentPath() []SearchElement {
	copyPath := make([]SearchElement, len(se.currentPath))
	copy(copyPath, se.currentPath)
	return copyPath
}

func (se *SearchEngine) IsStopped() bool {
	return se.stopped
}

func (se *SearchEngine) SetStopped(stopped bool) {
	se.stopped = stopped
}
