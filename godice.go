//
// Copyright (c) 2018 Steven Roberts <sroberts@fenderq.com>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//

package main

import (
	"bufio"
	"crypto/rand"
	"flag"
	"fmt"
	"log"
	"math/big"
	"os"
	"strconv"
	"strings"
)

type DicewareEntry struct {
	num int
	word string
}

func main() {
	var count int
	var fname string
	var numWords int

	// Get command-line arguments.
	flag.IntVar(&count, "c", 1, "number of words")
	flag.StringVar(&fname, "f", "", "diceware file")
	flag.IntVar(&numWords, "n", 0, "number of words")
	flag.Parse()

	if fname == "" || numWords == 0 {
		flag.PrintDefaults()
		os.Exit(1)
	}

	// Load diceware file.
	list := loadDiceware(fname)

	// Print diceware passphrases.
	for i := 0; i < count; i++ {
		printDiceware(list, numWords)
	}
}

func printDiceware(list []DicewareEntry, numWords int) {
	for i := 0; i < numWords; i++ {
		// Generate random index.
		max := int64(len(list)) - 1
		index, err := rand.Int(rand.Reader, big.NewInt(max))
		if err != nil {
			log.Fatal(err)
		}
		// Print random word.
		fmt.Printf("%s", list[index.Int64()].word)
		if i == numWords - 1 {
			fmt.Printf("\n")
		} else {
			fmt.Printf(" ")
		}
	}
}

// Line format: NUM\tWORD\n
func NewDicewareEntry(line string) DicewareEntry {
	d := DicewareEntry{}
	// Split on the tab character.
	s := strings.Split(line, "\t")
	d.num, _ = strconv.Atoi(s[0])
	d.word = s[1]

	return d
}

func loadDiceware(fname string) []DicewareEntry {
	// Open the file.
	list := []DicewareEntry{}
	file, err := os.Open(fname)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	// Read lines, create diceware entry, add to list.
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		d := NewDicewareEntry(line)
		list = append(list, d)
	}

	return list
}
