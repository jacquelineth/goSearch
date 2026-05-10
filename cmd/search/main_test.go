package main

import (
	"bytes"
	"io"
	"os"
	"testing"
)

func TestPrintUsage(t *testing.T) {
	oldStdout := os.Stdout
	r, w, err := os.Pipe()
	if err != nil {
		t.Fatalf("failed to create pipe: %v", err)
	}
	defer func() {
		os.Stdout = oldStdout
		_ = r.Close()
	}()
	os.Stdout = w

	outputCh := make(chan []byte)
	go func() {
		var buf bytes.Buffer
		_, _ = io.Copy(&buf, r)
		outputCh <- buf.Bytes()
	}()

	printUsage()
	_ = w.Close()

	output := <-outputCh

	expected := "Usage: search <rootPath> <filePattern> [<searchString>]"
	if !bytes.Contains(output, []byte(expected)) {
		t.Errorf("printUsage() = %q, want %q", string(output), expected)
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
		{[]string{"cmd", "rootPath", "filePattern", "extra", "another"}, true},
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
