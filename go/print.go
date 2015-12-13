package main

import (
	"fmt"
	"unicode/utf8"
)

func main() {
	const sample = "\xbd\xb2\x3d\xbc\x20\xe2\x8c\x98"

	fmt.Println("Println:")
	fmt.Println(sample)

	fmt.Println("Byte loop:")
	for i := 0; i < len(sample); i++ {
		fmt.Printf("%x ", sample[i])
	}
	fmt.Printf("\n")

	fmt.Println("Printf with %x:")
	fmt.Printf("%x\n", sample)

	fmt.Println("Printf with % x:")
	fmt.Printf("% x\n", sample)

	fmt.Println("Printf with %q:")
	fmt.Printf("%q\n", sample)

	fmt.Println("Printf with %+q:")
	fmt.Printf("%+q\n", sample)

	fmt.Println("Printf with slice:")
	fmt.Printf("% x\n", sample[0:5])

	fmt.Println("Byte loop: %q")
	for i := 0; i < len(sample); i++ {
		fmt.Printf("%q ", sample[i])
	}
	fmt.Printf("\n")

	const nihongo = "日本語⌘"
	for index, runeValue := range nihongo {
		fmt.Printf("%x\n", index)
		fmt.Printf("%#U starts at byte position %d\n", runeValue, index)
	}
	for i := 0; i < len(nihongo); i++ {
		fmt.Printf("%x ", nihongo[i])
	}
	fmt.Printf("\n")

	const nihongo1 = "日本語"
	for i, w := 0, 0; i < len(nihongo1); i += w {
		runeValue, width := utf8.DecodeRuneInString(nihongo1[i:])
		fmt.Printf("%#U starts at byte position %d\n", runeValue, i)
		w = width
	}
}
