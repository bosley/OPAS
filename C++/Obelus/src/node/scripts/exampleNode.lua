-- example node

function Request(requestJsonString)

    --             IP        PORT  DATA (Must be JSON)
    ObelusWrite("127.0.0.1", 8000, "{\"hpic\": \"getDesc\" }")
    
    ObelusWrite("127.0.0.1", 8000, "{\"hpic\": \"getVersion\" }")
    
    ObelusWrite("127.0.0.1", 4095, "{\"cliResp\": \"YAY!\" }")

    return 0
end