object "TestAdd" {
    code {
        mstore(0, add(0x12, 0x25))
        return(0, 32)
    }
}