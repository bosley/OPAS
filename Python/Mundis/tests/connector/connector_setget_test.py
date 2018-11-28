
import connector

if __name__ == '__main__':
    con = munconnect("127.0.0.1", 9001, "test", "a_basic_password", 'test_db')
    con.log_response = True
    con.connect()

    result = con.set({
        "key": "banner_image",
        "value": "/tmp/banner.png"
    })
    #print(result)

    result = con.set({
        "key": "home_gallery",
        "value": [
            "/tmp/gallery/1.png",
            "/tmp/gallery/2.png",
            "/tmp/gallery/3.png",
        ]
    })
    #print(result)

    result = con.set({
        "key": "my_dict",
        "value": {
            "key1": "val1",
            "key2": "val2"
        }
    })
    #print(result)

    for key in con.get({}):
        print(key, ":", con.get({
            "key": key
        }))

    result = con.setli({
        "key": "home_gallery",
        "value": "/tmp/gallery/new/90.png",
        "index": 1
    })
    #print(result)

    print(con.get({
        "key": "home_gallery"
    }))

    result = con.setli({
        "key": "home_gallery",
        "value": "/tmp/gallery/new/1000.png",
        "index": 10,
        "overflow": "append"
    })
    #print(result)

    print(con.get({
        "key": "home_gallery"
    }))

    result = con.setli({
        "key": "home_gallery",
        "value": "/tmp/gallery/new/eotw.png",
        "index": 12,
        "overflow": "expand"
    })
    #print(result)

    print(con.get({
        "key": "home_gallery"
    }))

    result = con.setdv({
        "key": "my_dict",
        "value": ("key2", "A WHOLE NEW VALUE")
    })
    #print(result)

    print(con.get({
        "key": "my_dict"
    }))

    con.disconnect()