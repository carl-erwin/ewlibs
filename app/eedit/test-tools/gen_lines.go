package main

import "os"
import "fmt"
import "strconv"

func main() {

	args := os.Args

	if len(args) != 4 {
		fmt.Println("usage: ", args[0], "start end width")
		os.Exit(1)
	}

	start := parseInt(args[1])
	end   := parseInt(args[2])
	width := parseInt(args[3])

	s := genLine(width)
	genLines(s, start, end)
}

func parseInt(s string) uint64 {

	val, err := strconv.ParseUint(s, 0, 64)
	if err != nil {
		// handle error
		fmt.Println(err)
		os.Exit(1)
	}

	return val;
}


func genLine(width uint64) string {

     var s string

     for i := uint64(0); i < width; i++ {
     	 s += "x"
     }

     return s;
}

func genLines(linePattern string, start uint64, end uint64) {

     for i := start; i <= end; i++ {
         fmt.Println(i, linePattern)
     }
}
