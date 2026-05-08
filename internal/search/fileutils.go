package search

import (
  "bufio"
  "io"
  "os"
  "path/filepath"
  "runtime"
  "strings"
)

var DefaultZipExtensions = []string{"zip", "jar", "war", "ear", "rar", "pak", "ods", "odp", "odg", "odt"}

func LoadConfigProperties(path string) (map[string]string, error) {
  props := make(map[string]string)
  f, err := os.Open(path)
  if err != nil {
    return props, err
  }
  defer f.Close()

  scanner := bufio.NewScanner(f)
  for scanner.Scan() {
    line := strings.TrimSpace(scanner.Text())
    if line == "" || strings.HasPrefix(line, "#") || strings.HasPrefix(line, "!") {
      continue
    }
    if idx := strings.IndexAny(line, "=:"); idx >= 0 {
      key := strings.TrimSpace(line[:idx])
      value := strings.TrimSpace(line[idx+1:])
      props[key] = value
    }
  }
  return props, scanner.Err()
}

func LoadZipExtensions(properties map[string]string) map[string]struct{} {
  value := properties["org.javatb.search.zip.extensions"]
  if strings.TrimSpace(value) == "" {
    value = strings.Join(DefaultZipExtensions, " ")
  }
  result := make(map[string]struct{})
  for _, token := range strings.Fields(value) {
    ext := strings.ToLower(strings.TrimSpace(token))
    if ext != "" {
      result[ext] = struct{}{}
    }
  }
  return result
}

func FileExtension(name string) string {
  idx := strings.LastIndex(name, ".")
  if idx < 0 {
    return ""
  }
  ext := name[idx+1:]
  if strings.ContainsAny(ext, "/\\") {
    return ""
  }
  return ext
}

func FileShortName(path string) string {
  return filepath.Base(path)
}

func ReadTextFile(filename string) (string, error) {
  f, err := os.Open(filename)
  if err != nil {
    return "", err
  }
  defer f.Close()
  return ReadTextFileFromReader(f)
}

func ReadTextFileFromReader(r io.Reader) (string, error) {
  scanner := bufio.NewScanner(r)
  var builder strings.Builder
  for scanner.Scan() {
    builder.WriteString(scanner.Text())
    builder.WriteString("\n")
  }
  return builder.String(), scanner.Err()
}

func ZipEntryAsBytes(r io.Reader) ([]byte, error) {
  return io.ReadAll(r)
}

func CheckEnoughMemory(size int64) bool {
  if size <= 0 {
    return true
  }
  if size < 50*1024*1024 {
    return true
  }
  var m runtime.MemStats
  runtime.ReadMemStats(&m)
  available := int64(m.Sys - m.Alloc)
  return available >= 3*size
}
