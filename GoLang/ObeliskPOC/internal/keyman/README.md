# Keyman

Key manager for ecdsa keys

### Create keys:

```go
    k.GenerateKeys("/tmp/key_name")
```
    Dumps keys to :
        "/tmp/key_name_Public.key" 
        "/tmp/key_name_Private.key"

    This WILL overwrite existing keys in this location

### Load keys:
```go
    k.LoadKeys("/tmp/key_name")
```

    Loads keys from :
        "/tmp/key_name_Public.key" 
        "/tmp/key_name_Private.key"

### Sign:
```go
    k.Sign("some data")

```
    Signs the data given the keymans loaded or generated keys,
    and returns a KeymanSignature struct

### Verify
```go
    k.Verify(PublicKey, KeymanSignature)
```

    Ensures that a given KeymanSignature's ecdsa big.Int keys
    originated from the given PublicKey and data within
    keymanSignature