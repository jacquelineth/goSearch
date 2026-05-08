package search

import (
	"archive/zip"
	"os"
	"path/filepath"
	"time"
)

type SearchElementType int

const (
	FileType SearchElementType = 1 << iota
	FolderType
	ArchiveType
	EntryType
)

type SearchElement struct {
	Name           string
	Type           SearchElementType
	Path           []SearchElement
	Size           int64
	CompressedSize int64
	LastModified   time.Time
	Attributes     string
}

func NewSearchElement(name string, typ SearchElementType, path []SearchElement) SearchElement {
	copied := make([]SearchElement, len(path))
	copy(copied, path)
	return SearchElement{
		Name:       name,
		Type:       typ,
		Path:       copied,
		Attributes: "",
	}
}

func NewSearchElementFromFile(path string, typ SearchElementType) (SearchElement, error) {
	info, err := os.Stat(path)
	if err != nil {
		return SearchElement{}, err
	}
	attributes := []rune{' ', ' ', ' ', ' '}
	if info.Mode().IsRegular() {
		if info.Mode().Perm()&0400 != 0 {
			attributes[0] = 'R'
		} else {
			attributes[0] = '.'
		}
		if info.Mode().Perm()&0200 != 0 {
			attributes[1] = 'W'
		} else {
			attributes[1] = '.'
		}
		if info.Mode().Perm()&0100 != 0 {
			attributes[2] = 'X'
		} else {
			attributes[2] = '.'
		}
	} else {
		attributes[0] = '.'
		attributes[1] = '.'
		attributes[2] = '.'
	}
	if info.Name()[0] == '.' {
		attributes[3] = 'H'
	} else {
		attributes[3] = '.'
	}
	return SearchElement{
		Name:           path,
		Type:           typ,
		Path:           nil,
		Size:           info.Size(),
		CompressedSize: -1,
		LastModified:   info.ModTime(),
		Attributes:     string(attributes),
	}, nil
}

func NewSearchElementFromZipFile(entry *zip.File, typ SearchElementType, path []SearchElement) SearchElement {
	copied := make([]SearchElement, len(path))
	copy(copied, path)
	modified := entry.Modified
	if modified.IsZero() {
		modified = time.Time{}
	}
	return SearchElement{
		Name:           entry.Name,
		Type:           typ,
		Path:           copied,
		Size:           int64(entry.UncompressedSize64),
		CompressedSize: int64(entry.CompressedSize64),
		LastModified:   modified,
		Attributes:     "R...",
	}
}

func (e SearchElement) IsFile() bool {
	return e.Type&FileType != 0
}

func (e SearchElement) IsFolder() bool {
	return e.Type&FolderType != 0
}

func (e SearchElement) IsArchive() bool {
	return e.Type&ArchiveType != 0
}

func (e SearchElement) IsArchiveEntry() bool {
	return e.Type&EntryType != 0
}

func (e SearchElement) String() string {
	parts := make([]string, len(e.Path)+1)
	for i, p := range e.Path {
		parts[i] = p.Name
	}
	parts[len(e.Path)] = e.Name
	return filepath.Join(parts...)
}
