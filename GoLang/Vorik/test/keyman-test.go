package main

import (
    "fmt"
    km "github.com/joshbosley/Vorik/keyman"
)

func main() {
	var str []byte
	str = []byte("This is some data that can be signed by the keyman")

	var k km.Keyman
	k.GenerateKeys("/tmp/keyManTest")

	fmt.Print("private key: ")
	fmt.Println(k.Private)
	fmt.Println()
	fmt.Print("public key: ")
	fmt.Println(k.Public)
	fmt.Println()

	ks, err := k.Sign(str)
	if err != nil {
		fmt.Printf("signing error: %s\n", err)
	}

	fmt.Println(ks.SignedData)

	verify := k.Verify(k.Public, ks)
	fmt.Printf("signature verification result (true?): %t\n", verify)

	ks.SignedData = []byte("some other text that isnt the original data")
	verify = k.Verify(k.Public, ks)

	fmt.Printf("signature verification result (false?): %t\n", verify)

	fmt.Println("Read in keys from file:")

	k.LoadKeys("/tmp/keyManTest")

	ks, err = k.Sign(str)
	if err != nil {
		fmt.Printf("signing error: %s\n", err)
	}

	verify = k.Verify(k.Public, ks)
	fmt.Printf("signature verification result (true?): %t\n", verify)

	ks.SignedData = []byte("some other text that isnt the original data")

	verify = k.Verify(k.Public, ks)
	fmt.Printf("signature verification result (false?): %t\n", verify)
	
}
