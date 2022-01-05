
#from libs.pyquaternion import Quaternion
import numpy as np
from libs import cadnano_utils as cu
from libs import base
from libs.readers import LorenzoReader

class Options(object):

	def __init__(self):
		object.__init__(self)
		
		self.seed = None
		self.conf_file_name_in = None
		self.top_file_name_in = None
		
	def check(self):
		return True

def oxDNA_to_rpoly(opts):
    # Read file and store into conf and top lists

    # stores position, base position, versor, velocity
    # stores backbone links information (strand idx, base, 3' neightbor idx, 5' neighbor idx)
	myreader = LorenzoReader(opts.top_file_name_in,opts.conf_file_name_in)
	mysystem = myreader.get_system()
	print("N_nucleotides: ",mysystem.get_N_Nucleotides(),"N_strands: ",mysystem.get_N_strands())
    #confFile = open(opts.conf_file_name_in, 'r') # configuration file (individual nucleotide information): position, base position, versor, velocity, angular velocity
    #topFile = open(opts.top_file_name_in, 'r') # topology file (sugar-phosphate backbone links information): 
    #counter = 0
	mysystem.map_nucleotides_to_strands()
	"""
    try: 
        for line in confFile:
            if counter >= 3: #Ignoring header lines
                conf.insert(counter - 3, line.split(' ')[0:9]) #velocity and angular velocity are not relevant for rpoly, and are hence cut off
            counter+=1
        counter = 0
        for line in topFile:
            if counter >= 1: #Ignoring header lines
                top.insert(counter-1, line.split(' '))
            else:
                nn, ns = int(line.split(' ')[0]), int(line.split(' ')[1]) #Nof nucleotides and strands
            counter+=1
    except Exception:
        print('Failed to read file')

    if (len(conf) != nn):
        print("Invalid oxDNA model: number of nucleotides in .oxdna file doesn't match number of nucleotides in .top file")
    pos = []
	"""
    #for i, row in enumerate(conf):

	


# Python/C++ API can conveniently call a function from a module but not run the module as a whole. if __main__: -> def main():
def main(argv):
	#if len(argv) < 1:
	#	print_usage()
    # print("Hello from Python\n")
	# arguments not passed through sys.argv
	#opts = parse_options()
    opts = Options()
    #opts.conf_file_name_in = "../workspace/star-4split.rpoly.oxdna"
	#opts.top_file_name_in = "../workspace/star-4split.rpoly.top"
    opts.seed = None
    opts.conf_file_name_in = argv[0]
    opts.top_file_name_in = argv[1]
    if opts.seed is not None:
    	np.random.seed(opts.seed)
    oxDNA_to_rpoly(opts)
    return 0





    


    
    

