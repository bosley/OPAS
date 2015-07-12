__author__ = 'Josh Bosley'

# List of >= 1980 Weights
pre_1980 = []
# List of <= 1990 Weights
post_1990 = []
# List of tuples of ranges
ranges = []


class RangeInstance():

    def __init__(self):
        self.given_range = (0.00, 0.00)
        self.weights_matched = []
        self.number_of_matches = 0
        self.which_year = ''


class YearInstance():

    def __init__(self):
        self.year = ''
        self.total = 0
        self.list_of_ranges_with_hits = [RangeInstance()]
        self.list_of_ranges_with_hits.pop()

# Instances of each year tested against
year_1980 = YearInstance()
year_1990 = YearInstance()


def populate_array(file_name):

    with open(file_name) as fp:

        print "\n Loading " + file_name.split('.')[0]

        for line in fp:

            if file_name.split('.')[0] == '1980':

                pre_1980.append(float("{0:.2f}".format(float(line.strip('\n')))))

            if file_name.split('.')[0] == '1990':

                post_1990.append(float("{0:.2f}".format(float(line.strip('\n')))))

            if file_name.split('.')[0] == 'ranges':

                item = line.split("-")
                block = (float(item[0].strip('\n')), float(item[1].strip('\n')))
                ranges.append(block)
    return


def add_hit_to_range(given_range, weight, year):

    is_found = False
    group = YearInstance()

    if year == '1980':
        group = year_1980

    elif year == '1990':
        group = year_1990

    else:
        print " Error setting group "
        return

    # Go through list of instances
    for instance in group.list_of_ranges_with_hits:
        # If the range given is the range in instance, add weight and increment tally
        if instance.given_range == given_range:
            instance.weights_matched.append(weight)
            instance.number_of_matches += 1
            is_found = True

            group.total += 1

    if not is_found:
        # Range has not yet been added to the instance list, so add it an its weight and tally
        new_instance = RangeInstance()
        new_instance.given_range = given_range
        new_instance.weights_matched.append(weight)
        new_instance.number_of_matches += 1
        group.list_of_ranges_with_hits.append(new_instance)

        group.total += 1

    return


def test_ranges():

    # For every given range
    for r in ranges:
        # For all weights in 1980 weights
        for weight in pre_1980:

            # If weight is in range given, add to list_of_ranges_with_hits
            if (weight >= r[0]) and (weight <= r[1]):
                add_hit_to_range(r, weight, '1980')

        # For all weights in 1990 weights
        for weight in post_1990:
            # If weight is in range given, add to list_of_ranges_with_hits
            if (weight >= r[0]) and (weight <= r[1]):
                add_hit_to_range(r, weight, '1990')

    return


def display_results():

    print " There where %s ranges with weights associated in <= 1980 group. \n\n" \
          % str(len(year_1980.list_of_ranges_with_hits))

    print " 1980 comparisons : %s\n" % str(year_1980.total)


    for instance in year_1980.list_of_ranges_with_hits:
        print "\t Instance range : %s \n" % str(instance.given_range)
        print " %s number of hits \n" % str(instance.number_of_matches)
        print " Weights Found : \n"
        print instance.weights_matched
        print "\n"

    print "\n -------------------------------------------------- \n"
    print " There where %s ranges with weights associated in >= 1990 group. \n\n" \
          % str(len(year_1990.list_of_ranges_with_hits))

    print " 1990 comparisons : %s\n" % str(year_1990.total)

    for instance in year_1990.list_of_ranges_with_hits:
        print "\t Instance range : %s \n" % str(instance.given_range)
        print " %s number of hits \n" % str(instance.number_of_matches)
        print " Weights Found : \n"
        print instance.weights_matched
        print "\n"

    return


if __name__ == '__main__':

    populate_array('1980.txt')
    populate_array('1990.txt')
    populate_array('ranges.txt')

    print "\n Testing Ranges\n"
    test_ranges()

    display_results()
