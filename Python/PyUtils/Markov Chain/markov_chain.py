__author__ = 'Josh Allen Bosley'

import time
from random import randrange

class Node:

    def __init__(self, name, rates):
        self.node_name = name
        self.node_rates = rates

class MarkovChain:

    def __init__(self, list_of_nodes, record_state):

        self.current_state = 0
        self.rs = record_state
        self.state_record = []

        self.node_list = list_of_nodes
        for i in range(len(self.node_list)):
            if len(self.node_list[i].node_rates) != len(self.node_list):
                print "Unequal number of node_rates in node [%s]." % self.node_list[i].node_name
                exit(1)

    def start_sequence(self):

        while 1:
            temp = []
            roll = randrange(0,100)

            # Create temp list of rates from current list, and find which is closest to random number
            if run_is_example:
                print "current_state=%s" % self.node_list[self.current_state].node_name

            for i in range(len(self.node_list[self.current_state].node_rates)):

                temp.append(int(self.node_list[self.current_state].node_rates[i]*100))

            self.current_state = temp.index( min(temp, key=lambda x:abs(x-roll)) )

            if run_is_example:
                print "roll=%s , selected=%s" % (roll,self.current_state)

            if self.rs == 1:
                self.state_record.append(self.node_list[self.current_state].node_name)

            temp[:] = []

            if run_is_example:
                time.sleep(5)
        return


# Example :
run_is_example = False
if __name__ == '__main__':

    run_is_example = True
    node_list = [Node for i in range(3)]
    node_list[0] = Node("State1", [0.5, 0.8, 0.3])
    node_list[1] = Node("State2", [0.8, 0.5, 0.3])
    node_list[2] = Node("State3", [0.3, 0.8, 0.5])

    # Pass node list, and tell to record state list, and go forever
    chain_states = MarkovChain(node_list, 1)

    chain_states.start_sequence()

