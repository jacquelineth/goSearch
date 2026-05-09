package main

import (
	"fmt"
	"os"
	"path/filepath"

	"goSearch/internal/search"
)

var exit = os.Exit

func printUsage() {
	fmt.Println("Usage: search <rootPath> <filePattern> [<searchString>]")
	fmt.Println("")
	fmt.Println("where:")
	fmt.Println("")
	fmt.Println("  - <rootPath> is the path to a folder or an archive")
	fmt.Println("    compatible archive formats include: .zip, .jar, .ear, .war and .rar")
	fmt.Println("")
	fmt.Println("  - <filePattern> is a pattern to search for in each archived file name")
	fmt.Println("    the wildcards '*' and '?' can be used")
	fmt.Println("")
	fmt.Println("  - <searchString> is an optional string to search for in the visited files")
	fmt.Println("")
	fmt.Println("  - example: search ../lib *.class listener")
	fmt.Println("    searches for all .class files in ../lib and subfolders that contain the string 'listener'")
}

func main() {
	args := os.Args[1:]
	if len(args) < 1 || len(args) > 3 {
		printUsage()
		exit(1)
		return
	}

	rootPath := args[0]
	filePattern := ""
	searchTerm := ""
	if len(args) > 1 {
		filePattern = args[1]
	}
	if len(args) > 2 {
		searchTerm = args[2]
	}

	properties, err := search.LoadConfigProperties(filepath.Join("config", "config.properties"))
	if err != nil {
		properties = map[string]string{}
	}
	ext := search.LoadZipExtensions(properties)

	engine := search.NewSearchEngine(rootPath, filePattern, searchTerm, ext)
	matches := []search.SearchElement{}
	err = engine.Search(func(event search.SearchEvent) {
		if event.Type == search.SearchEventTypeMatch {
			matches = append(matches, event.Element)
			fmt.Println(event.Element.String())
		}
	})
	if err != nil {
		fmt.Fprintf(os.Stderr, "Search failed: %v\n", err)
		exit(1)
	}
	if len(matches) == 0 {
		fmt.Println("No matches found")
	} else {
		plural := ""
		if len(matches) > 1 {
			plural = "es"
		}
		fmt.Printf("Found %d match%s\n", len(matches), plural)
	}
}
