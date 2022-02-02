import random, math


complement = {"A": "T", "T": "A", "G": "C", "C": "G"}


def main(argv):
    #Generate random secondary structure for testing and a pairing dictionary based on it.
    #The input for the algorithm is only the pairing dictionary.
    ss = generate_ss(40)
    pairs = ss_to_dict(ss)
    #print("Generated pairs: \n")
    print(len(pairs.keys()))
    #Generate random splits. The algorithm only considers the primary structure as a single strand, but the
    #splits-vector can be used to split it into multiple strands.
    splits = sorted(set([0, len(ss)] + [random.randint(0, len(ss) - 1) for i in range(random.randint(5,10))]))
    
    ps = generate_primary(pairs)
    print(len(ps))
    

    #Results:
    print(f"Primary structure length: {len(ps)}")
    print(f"Generated primary structure GC-content: {(ps.count('G') + ps.count('C')) / len(ps):.2f}")
    print(f"Longest repeat: {find_longest_repeat(ps)}")
    print("Generated strands:")
    for i in range(1,len(splits)):
        print(ps[splits[i-1]: splits[i]])


def generate_primary(pairs, target_gc = 0.5, sa_fac = 0.9):
    """
    Generates the single-stranded primary structure based on the pairing-dictionary
    Args:
        pairs
        target_gc - target GC-concentration
        sa_fac - simulated annealing cooling factor
    """
    
    p = PrimaryStructure(pairs, target_gc)
    p.optimise(fac = sa_fac)
    ps = "".join(p.primary)
    return ps


def find_longest_repeat(ps):
    """ Finds the longest repeated sub-sequence in the given string using a suffix-tree.
    """
    from suffix_trees import STree
    st = STree.STree(ps)
    deepest = max(st.root._get_leaves(), key = lambda x : x.parent.depth).parent.depth
    return deepest


### -- for testing -- ###
def generate_ss(depth):
    t = ["s"]
    for i in range(depth):
        for j, s in enumerate(t):
            if s == "s":
                if random.random() > 0.75: t[j] = "ss"
                else: t[j] = "(s)"
        t = list("".join(t))

    t = "".join(t).replace("s",".")
    return t


def ss_to_dict(ss):
    d = {}
    stack = []
    for i, s in enumerate(ss):
        if s == "(": stack.append(i)
        elif s == ")":
            j = stack.pop()
            d[i] = j; d[j] = i
        else:
            d[i] = i
    return d
### -- -- ###


class ListDict():
    """
    A combination of a list and a dictionary. Allows for a linear-time random-sampling and linear-time
    addition/removal of specific elements.
    """
    def __init__(self):
        self.d = {}
        self.items = []

    def add(self, item):
        if item in self.d: return
        self.items.append(item)
        self.d[item] = len(self.items)-1

    def remove(self, item):
        position = self.d.pop(item)
        last_item = self.items.pop()
        if position != len(self.items):
            self.items[position] = last_item
            self.d[last_item] = position

    def get_random(self):
        return random.choice(self.items)

    def __len__(self):
        return len(self.items)


class PrimaryStructure():
    """ A single-stranded DNA primary structure.
    """

    def __init__(self, pairs, target_gc = 0.5):
        """
        Args:
            pairs - dictionary
            target - target GC-concentration

        """
        # Approximation for the length of the longest repeated sub-string
        # Since the repeats are stored simply in a dictionary instead of in a structure like a suffix-tree,
        # only the repeats of the relevant range should be stored.
        self.min_size = math.ceil(2*((0.5 - abs(target_gc - 0.5)) *
                                     math.log(len(pairs), 4) + abs(target_gc - 0.5) * math.log(len(pairs), 2)))
        self.max_size = self.min_size + 5
        # The fitness score associated with the length of a repeated sub-string.
        self.interval_scores = {i : i ** 2 for i in range(self.min_size, self.max_size + 1)}
        self.score_trace = [] # For plotting.

        self.pairs = pairs # pairing-dictionary.
        self.target_gc = target_gc # target GC-concentration

        self.current_gc = 0 # Current number of GC-bases. Not actually the concentration.. for some reason
        self.primary = self.__seed_primary() # A list of the current primary structure.
        self.repeats_ds = self.__populate_repeats() # A dictionary containing all sub-sequences.
        self.offenders_ds = self.__populate_offenders() # A listdict containing all repeated sub-sequences.

    def __seed_primary(self):
        """ A random initial guess for the primary structure
        """
        primary = ["A"] * len(self.pairs)
        for k, v in self.pairs.items():
            b = self.__get_random_base()
            if b in "GC": self.current_gc += 1
            primary[k] = b
            primary[v] = complement[b]
        return primary

    def __populate_offenders(self):
        """
        Populates the offending sub-sequences, i.e., all of whom are repeated more than once.
        """
        offenders = {i : ListDict() for i in range(self.min_size, self.max_size + 1)}
        for i, s in enumerate(self.primary):
            for ivl in self.repeats_ds.keys():
                s = "".join(self.primary[i:i+ivl])
                if len(self.repeats_ds[ivl][s]) > 1: offenders[ivl].add(s)
        return offenders



    def __populate_repeats(self):
        """
        Stores every single sub-sequence into the repeats-dictionary.
        """
        repeats = {i : {} for i in range(self.min_size, self.max_size + 1)}
        for i, s in enumerate(self.primary):
            for ivl in repeats.keys():
                repeats[ivl].setdefault("".join(self.primary[i:i+ivl]), set()).add(i)
        return repeats


    def __set_base(self, idx, symbol):
        """
        Sets a base at idx to symbol. Also updates the repeats and offenders.
        """
        for ivl in self.repeats_ds.keys():
            for i in range(ivl):
                j = idx - i
                if j < 0 or j + ivl > len(self.primary): continue
                t = self.primary[j:j+ivl]

                #remove old repeats
                s = "".join(t)
                self.repeats_ds[ivl][s].remove(j)
                if len(self.repeats_ds[ivl][s]) == 1: self.offenders_ds[ivl].remove(s)
                if len(self.repeats_ds[ivl][s]) == 0: self.repeats_ds[ivl].pop(s)
                #add new repeats
                t[i] = symbol
                s = "".join(t)
                self.repeats_ds[ivl].setdefault(s, set()).add(j)
                if len(self.repeats_ds[ivl][s]) > 1: self.offenders_ds[ivl].add(s)
        if self.primary[idx] not in "GC" and symbol in "GC": self.current_gc += 1
        elif self.primary[idx] in "GC" and symbol not in "GC": self.current_gc -= 1
        self.primary[idx] = symbol

    def __get_offender(self):
        """
        Selects a random offending sub-sequence, i.e., one that is repeated more than once. Prioritises
        the longest sub-sequences. Returns a random index of that sub-sequence.
        """
        i = 0
        for ivl in reversed(self.repeats_ds.keys()):
            if len(self.offenders_ds[ivl]) > 0:
                t = self.offenders_ds[ivl].get_random()
                i = random.choice(list(self.repeats_ds[ivl][t]))
                i += random.randint(0, ivl - 1)
                break
        return i

    def __get_random_base(self):
        """
        Returns a random base according to the target GC-concentration.
        """
        p = random.random()
        if p < self.target_gc:
            if p < 0.5 * self.target_gc: return "G"
            else: return "C"
        else:
            if p < 0.5 * (self.target_gc + 1): return "A"
            else: return "T"


    def get_score(self):
        """
        Calculates the fitness-score of the current primary structure by giving a weighted score to
        every single entry in the in the repeats-dictionary. The more entries there are, the fewer
        actually repeated sequences there can be. Probably not the best metric, but it works.
        """
        s = 0
        for ivl in self.repeats_ds.keys():
            s += self.interval_scores[ivl] * len(self.repeats_ds[ivl])
        # Give a penalty based on the current GC-content in relation to the target GC-content:
        s *= (1 - abs(self.current_gc / len(self.primary) - self.target_gc)) ** 2
        return s


    def optimise(self, T = 1000, ts = 5, fac = 0.9):
        """
        Simulated annealing.

        Args:
            T - initial temperature
            ts - number of iterations spent in a temperature
            fac - cooling factor
        """
        while T > 0.1:
            if ts > 0: ts -= 1
            else:
                ts = 5
                T = T * fac
            o_score = self.get_score()

            ## Get neighbour
            n_edits = 5 # Number of edits
            # Targeted edit. Only edit the offending sub-sequences. Completely random edits seem inefficient.
            edits = [(self.__get_offender(), self.__get_random_base()) for i in range(n_edits)]
            prev = [] # Store previous bases here in case the edits need to be reverted.
            for i, s in edits:
                prev.append((i, self.primary[i]))
                self.__set_base(i, s)
                j = self.pairs[i]
                if i != j:
                    prev.append((j, self.primary[j]))
                    self.__set_base(j, complement[s])
            n_score = self.get_score()

            try: p = math.exp((n_score - o_score) / T)
            except: p = 1
            if random.random() > p:
                # Neighbour not accepted. Revert into the unedited state by using the prev-list.
                for i, s in prev:
                    self.__set_base(i, s)

            self.score_trace.append(self.get_score())




if __name__ == "__main__":
    import sys
    main(sys.argv)
