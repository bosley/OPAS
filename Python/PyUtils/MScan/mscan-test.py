___author__ = 'Josh Allen Bosley'

from mscan import scan

if __name__ == '__main__':

    # Scan of single port, printing information
    print "\n\t String returned for printing information\n"
    print scan('104.236.235.37', '21', 0)

    # Scan of single port, returning list of information of ip
    print "\n\t List returned for use, no host information\n "
    print scan('104.236.235.37', '21', 1)

    # Scan of single port, returning list of information with ip
    print "\n\t List returned for use, with host information\n "
    print scan('104.236.235.37', '21', 2)