# pdklex
multipurpose list externals for pure data

- dklmunge - list processor/munger. Here are the methods I've added so far:
    - none - do nothing
    - avg - sums all float args and divides by number of ALL elts
    - everyn (f=n, f=offset) - output every nth elt with offset
    - faro - faro shuffle
    - fyshuf - fisher-yates shuffle
    - group (f) - output in groups of f prepended by l0,l1,.....
    - indexof (f/s) - returns index of given else -1
    - max - max of all float elts
    - min - min of all float elts
    - ref (f/s) - returns sublist starting at given f or s if found
    - relt (f list) - random elt selection: 1 arg specifies number (if given), rest specify weights
    - rev - reverse list
    - revrot (f) - reverse then rotate list
    - rot (f) - rotate list
    - rpt1 (f=idx, f=rpt, pairs) - takes pairs, repeats idx rpt times
    - sum - adds all float args together
    - prod - multiplies all float args together
