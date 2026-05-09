package main
// @Todo Review and correct 
import (
	"bytes"
	"os"
	"testing"
)

func TestPrintUsage(t *testing.T) {
	// Capture the output of printUsage
	output := &bytes.Buffer{}
	console := os.Stdout
	os.Stdout = output
	defer func() { os.Stdout = console }() // Restore stdout

	printUsage()

	expected := "Usage: search <rootPath> <filePattern> [<searchString>]"
	if !bytes.Contains(output.Bytes(), []byte(expected)) {
		t.Errorf("printUsage() = %q, want %q", output.String(), expected)
	}
}

func TestMainArgumentValidation(t *testing.T) {
	// Save original args
	originalArgs := os.Args
	defer func() { os.Args = originalArgs }()

	cases := []struct {
		args    []string
		wantErr bool
	}{
		{[]string{"cmd"}, true},
		{[]string{"cmd", "rootPath"}, false},
		{[]string{"cmd", "rootPath", "filePattern"}, false},
		{[]string{"cmd", "rootPath", "filePattern", "extra"}, true},
	}

	for _, c := range cases {
		os.Args = c.args

		// Capture the exit code
		code := 0
		exit = func(c int) { code = c } // Mock exit
		defer func() { exit = os.Exit }()

		main()
		if (code != 0) != c.wantErr {
			t.Errorf("main() with args %v, got error: %v, want error: %v", c.args, code != 0, c.wantErr)
		}
	}
}