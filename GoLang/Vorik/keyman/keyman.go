/*
	Josh A. Bosley
	Key manager for ecdsa keys

	Create keys:
		k.GenerateKeys("/tmp/key_name")

		Dumps keys to :
			"/tmp/key_name_Public.key" 
			"/tmp/key_name_Private.key"

		This WILL overwrite existing keys in this location

	Load keys:
		k.LoadKeys("/tmp/key_name")

		Loads keys from :
			"/tmp/key_name_Public.key" 
			"/tmp/key_name_Private.key"

	Sign:
		k.Sign("some data")

		Signs the data given the keymans loaded or generated keys,
		and returns a KeymanSignature struct

	Verify
		k.Verify(PublicKey, KeymanSignature)

		Ensures that a given KeymanSignature's ecdsa big.Int keys
		originated from the given PublicKey and data within
		keymanSignature
*/

package keyman

import (
        "crypto/ecdsa"
        "crypto/elliptic"
        "crypto/rand"
        "crypto/x509"
	"math/big"
	"os"
)

type KeymanSignature struct {
	SignedData []byte 
	r, s *big.Int
}

type Keyman struct {
	Public, Private []byte
	keyName string
}

func (k * Keyman) GenerateKeys(keyName string) {
	k.keyName = keyName
	charCheck := k.keyName[len(k.keyName)-1:]

	if charCheck == "/" {
		k.keyName = k.keyName[:len(k.keyName)-1]
	}

	private_key, _ := ecdsa.GenerateKey(elliptic.P224(), rand.Reader)
	k.Private, _ = x509.MarshalECPrivateKey(private_key)
	k.Public, _ = x509.MarshalPKIXPublicKey(&private_key.PublicKey)

	f, err := os.Create(keyName + "_Public.key")
	g, err := os.Create(keyName + "_Private.key")
	defer f.Close()
	defer g.Close()

	_, err = f.Write(k.Public)
	if err != nil {
		panic(err)
	}
	f.Sync()

	_, err = g.Write(k.Private)
	if err != nil {
		panic(err)
	}
	g.Sync()
}

func (k * Keyman) LoadKeys(keyName string) {
	charCheck := keyName[len(keyName)-1:]
	if charCheck == "/" {
		keyName = keyName[:len(keyName)-1]
	}
	k.Public = readBytes(keyName + "_Public.key")
	k.Private = readBytes(keyName + "_Private.key")
}

func (k * Keyman) Sign(hash []byte) (ks KeymanSignature, err error) {

	private_key, err := x509.ParseECPrivateKey(k.Private)

	ks = KeymanSignature{hash, big.NewInt(0), big.NewInt(0)}

	if err != nil {
		return ks, err
	}

	r, s, err := ecdsa.Sign(rand.Reader, private_key, hash)
	if err != nil {
		return ks, err
	}

	ks.r = r
	ks.s = s
	return ks, nil
}

func (k * Keyman) Verify(key []byte, ks KeymanSignature) (result bool) {
	pkey, err := x509.ParsePKIXPublicKey(key)
	if err != nil {
		return false
	}
	switch pkey := pkey.(type) {
	case *ecdsa.PublicKey:
		return ecdsa.Verify(pkey, ks.SignedData, ks.r, ks.s)
	default:
		return false
	}
}

func readBytes(location string) []byte{
	file, err := os.Open(location)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	// This assumes that the key will be within a 32*1024 buffer (definitely should be)
    buf := make([]byte, 32*1024)
	n, err := file.Read(buf)
	return buf[:n]
}
