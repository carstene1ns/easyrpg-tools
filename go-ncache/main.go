/*
 * c1 2024
 */

package main

import (
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"strings"
	"time"

	"encoding/json"
	"github.com/alexflint/go-arg"
	"github.com/kataras/golog"
	"golang.org/x/text/unicode/norm"
)

// helpers

func uniStringLower(input string) string {
	return strings.ToLower(norm.NFKC.String(input))
}

var extPriorities = map[string]int{
	// audio, order by size, smaller is better
	".wav":  0,
	".flac": 1,
	".mid":  2,
	".ogg":  3,
	".opus": 4,
	".mod":  5,
	/* graphics, first order by size,
	   but move png last, because of true color support */
	".bmp": 0,
	".xyz": 1,
	".png": 2,
	// prefer our xml over lcf
	".lmu": 0,
	".emu": 1,
	".lmt": 0,
	".emt": 1,
	".ldb": 0,
	".edb": 1}

func priorizeExt(first, second string) bool {
	prioFirst, ok1 := extPriorities[first]
	prioSecond, ok2 := extPriorities[second]
	if !(ok1 && ok2) {
		golog.Warn("No idea how to handle priority of extension ", first,
			" vs. ", second, ", keeping first.")
		return false
	}
	return prioSecond > prioFirst
}

// dir

func createCache(directory string, depth, maxDepth int) map[string]any {
	reservedKey := "_dirname"

	// create tree structure
	content := make(map[string]any)

	files, err := os.ReadDir(directory)
	if err != nil {
		golog.Error(err)
		return nil
	}

	// Setup dir content
	if depth != 0 {
		// Add internal key
		content[reservedKey] = filepath.Base(directory)
	}

	for _, f := range files {
		newDir := filepath.Join(directory, f.Name())
		fnameKey := uniStringLower(f.Name())

		if f.IsDir() {
			if depth >= maxDepth {
				golog.Debug("Skipping sub-directory:", newDir)
			} else {
				// Add directory
				content[fnameKey] = createCache(newDir, depth+1, maxDepth)
			}
		} else {
			// Skip internal key
			if f.Name() == reservedKey {
				golog.Warn("Skipping \"_dirname\": File conflicts with reserved keyword!")
			} else if f.Type().IsRegular() || f.Type()&fs.ModeSymlink == fs.ModeSymlink {
				// Add files
				ext := filepath.Ext(f.Name())
				baseName := strings.TrimSuffix(f.Name(), ext)

				//.ini and .po always need extensions
				// exfont is a special file in main directory
				if ext == ".ini" || ext == ".po" {
					golog.Debug("Skip renaming ", f.Name())
				} else if depth > 0 || baseName == "exfont" {
					fnameKey = uniStringLower(baseName)
				}

				// Check for overwritten value
				val, present := content[fnameKey].(string)
				if present {
					if priorizeExt(filepath.Ext(val), ext) {
						golog.Debug("Replacing extension from ", val, " to ", ext)
					} else {
						golog.Debug("Keeping extension of ", val, " (not changing to ", ext, ")")
						continue
					}
				}
				content[fnameKey] = f.Name()
			}
		}
	}

	return content
}

// json

func encodeJson(output string, input any, indent bool) {
	var outwriter *os.File

	if output == "" {
		outwriter = os.Stdout
		golog.Debug("Writing to stdout")
	} else {
		var err error
		outwriter, err = os.Create(output)
		if err != nil {
			golog.Fatal(err)
		}
		defer outwriter.Close()
	}

	enc := json.NewEncoder(outwriter)
	if indent {
		golog.Debug("Enabling pretty print")
		enc.SetIndent("", "  ")
	}
	enc.Encode(input)

	if output != "" {
		fmt.Printf("Directory cache has been written to \"%s\".\n", output)
	}
}

// cli

type args struct {
	InputDir       string `arg:"positional" default:"."`
	OutputFile     string `arg:"-o,--output" placeholder:"FILE" default:"index.json"`
	PrettyPrint    bool   `arg:"-p,--pretty" help:"Pretty print the JSON contents"`
	RecursionDepth int    `arg:"-r,--recurse" placeholder:"DEPTH" help:"Recursion depth" default:"3"`
	Verbosity      bool   `arg:"--verbose" help:"Explain what is being done"`
}

func (args) Description() string {
	return "JSON cache generator for EasyRPG Player ports"
}

func (args) Epilogue() string {
	return "It uses the current directory if not given as argument.\n" +
		"Homepage https://easyrpg.org/ - " +
		"Report bugs at: https://github.com/EasyRPG/Tools/issues"
}

var version string = "git"

func (args) Version() string {
	return "ncache " + version
}

// entry point

func main() {
	// setup cli
	var args args
	arg.MustParse(&args)

	// setup logger
	golog.SetOutput(os.Stderr)
	golog.SetTimeFormat("")
	golog.SetLevel("warn")
	if args.Verbosity {
		golog.SetLevel("debug")
	}

	// validate
	if args.RecursionDepth < 0 {
		golog.Fatal("Cannot set negative recursion depth!")
	}

	// generate cache
	golog.Info("Scanning filesystem")
	cache := createCache(args.InputDir, 0, args.RecursionDepth)

	// add metadata
	data := map[string]any{
		"metadata": map[string]any{
			"version": 2,
			"date":    time.Now().Format("2006-01-02"),
			"tool":    "go-ncache " + version},
		"cache": cache}

	// output json
	golog.Info("Writing JSON")
	encodeJson(args.OutputFile, data, args.PrettyPrint)
}
