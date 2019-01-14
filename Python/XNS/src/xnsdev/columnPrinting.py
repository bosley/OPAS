
from datetime import datetime
from xnsdisplay import cout

def coltest(nexus_info):
    
    # To ensure the column is at least as wide as the headers
    c = [2,2,2]
    uid_col = []
    name_col = []
    address_col = []
    location_col = []
    times = []

    for x in nexus_info:
        if len(x["uid"]) > c[0]:
            c[0] = len(x["uid"])
        if len(x["name"]) > c[1]:
            c[1] = len(x["name"])
        if len(x["location"]) > c[2]:
            c[2] = len(x["location"])
        uid_col.append(x["uid"])
        name_col.append(x["name"])
        address_col.append(x["location"])
        location_col.append(x["address"])
        times.append(x["activity"])

    for x in range(0,len(uid_col)):
        w = [
            " " * (c[0] - len(uid_col[x]) +2),
            " " * (c[1] - len(name_col[x])+2),
            " " * (c[2] - len(address_col[x])+2)
        ]
        # Color code
        difference = datetime.now() - datetime.strptime(times[x], "%Y-%m-%d %H:%M:%S.%f")
        minutes = divmod(difference.days * 86400 + difference.seconds, 60)[0]
    
        if minutes <= 5:
            dc = "lightgreen"
        elif minutes <= 10:
            dc = "yellow"
        else:
            dc ="fail"

        # Print each row
        cout(dc, (" " + uid_col[x] + w[0] + name_col[x] + w[1] + address_col[x] + w[2] + location_col[x]))
    return