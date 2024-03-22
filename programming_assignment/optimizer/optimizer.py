import re
from dominator_tree import dominator_tree
from utils import utils

def compute_dominance_frontier(root,idom,adj_list,df,dom_relation):
    s = set()
    for y in adj_list[root.name]:
        if y in idom:
            if idom[y] != root.name:
                s.update({y})
    for child  in root.children:
        if child is None:
            break
        _ = compute_dominance_frontier(child,idom,adj_list,df,dom_relation)
        for member in df[child.name]:
           # if root does not dominate memeber
            if check_domination(member,root.name,dom_relation) is False:
                s.update({member})
    df[root.name] = s
    return df
def check_domination(member, dominator,dom_relation):
    if dominator in dom_relation[member]:
        return True
    return False
def get_idom(dominance_realtion):
    idom = {}
    for block_num,dom in dominance_realtion.items():
        if block_num == 1:
            idom[block_num] = block_num
        else:
            second_max = utils.find_second_maximum(dom)
            idom[block_num] = second_max
    return idom

def get_dominance_relation(adj_list,blocks):
    """Obtains dominance relation of given blocks in CFG

    :param adj_list: adjacency list represenation of CFG
    :type adj_list: dict
    :param blocks: basic blocks of CFG
    :type blocks: dict
    :return: dictinoary containing dominance relation of the blocks
    :rtype: dict
    """
    dom = {key: list(range(1, len(blocks)-1)) for key in range(1, len(blocks)-1)}
    for block_num in blocks:
        if (block_num == 0) or (block_num == len(blocks)-1):
            continue
        if block_num == 1:
            dom[block_num] = [1]
            continue
        else:
            predecessors = find_predecessors(block_num,adj_list)
            predecessors_contribution = set()
            for node_num in predecessors:
                if len(predecessors_contribution) == 0:
                    for item in dom[node_num]:
                        predecessors_contribution.add(item)
                    continue
                current_set = set()
                for item in dom[node_num]:
                    current_set.add(item)
                predecessors_contribution.intersection(current_set)
        prev_set = {block_num}
        dom[block_num] = list(prev_set.union(predecessors_contribution))
    return dom

def find_predecessors(block_num,adj_list):
    """ Obtains predecsors of a given basic block in the CFG

    :param block_num: block number whose predecesor has to be determined
    :type block_num: int
    :param adj_list: adjacency list represenation of CFG
    :type adj_list: dict
    :return: list containing the predecessors
    :rtype: list
    """
    pred_set = set()
    for node in adj_list:
        for nbr in adj_list[node]:
            if nbr == block_num:
                pred_set.add(node)
    return list(pred_set)

def get_graph(blocks):
    """Given basic blocks of CFG, obtains adjacency list representation of CFG 

    :param blocks: basic blocks of CFG
    :type blocks: dict
    :return: adjacency list representation of CFG
    :rtype: dict
    """
    adjacency_list = {}
    for block in blocks:
        nghbrs = {}
        for line_index, line in enumerate(blocks[block]):
            contains_goto, target = find_goto_with_number(line)
            if contains_goto:
                block_num = find_block(target,blocks)
                replacement_string = str(block_num)+"_BLOCK"
                blocks[block][line_index]  = line.replace(str(target),replacement_string)
                nghbrs[block_num] = blocks[block_num]
        if block < len(blocks)-1:
            nghbrs[block+1] = (blocks[block+1])
        adjacency_list[block] = nghbrs
    return adjacency_list

def add_terminals(blocks):
    """Add entry and exit terminals to a list of blocks.
    :param blocks: The list of blocks to which terminals will be added.
    :type blocks: dict
    """
    blocks[0] = "ENTRY"
    blocks[len(blocks)] = "EXIT"

def find_block(target,cfg):
    """
    Find the block containing a specific line number.
    :param target: The line number to search for.
    :type target: int
    :param cfg: The control flow graph represented as a dictionary
    :type cfg: dict
    :return: Block number containing line, or -1 if not found.
    :rtype: int
    """
    num = 1
    for block in cfg:
        for _ in cfg[block]:
            if target == num:
                return block
            num+=1
    return -1
        
def get_blocks(lines):
    """Given a string of lines, partitions them to basic blocks of CFG

    :param lines: string of lines consisting of three address code
    :type lines: string
    :return: lines partitioned into basic blocks
    :rtype: dict
    """
    leaders = get_leaders(lines)
    blocks = {}
    block_counter = 1
    block = []
    for num in lines:
        if(leaders[num-1] is True):
            if block:
                blocks[block_counter] = block
                block_counter+=1
            block = []
            block.append(lines[num])
        else:
            block.append(lines[num])
            if (num == len(lines)):
                if block:
                   blocks[block_counter] = block
    return blocks

def get_leaders(lines):
    """Get the leaders in a sequence of lines.

    :param lines: The sequence of lines to analyze.
    :type lines: list
    :return: A list indicating whether each line is a leader.
    :rtype: list[bool]
    """
    leaders = [False for line in lines]
    branch = [False for line in lines]
    for num in lines:
        if num == 1:
            leaders[num-1] = True
            continue
        contains_goto, target = find_goto_with_number(lines[num])
        if contains_goto:
            leaders[target-1] = True
            branch[num-1] = True
        else:
            if ((branch[num-2] is True) and (num !=2)):
                leaders[num-1] = True
    return leaders

def find_goto_with_number(input_string):
    """Find the 'goto' statement with a specified number.

    :param input_string: The string to search for the 'goto' statement.
    :type input_string: str
    :return: A tuple indicating whether a 'goto' statement was found, number associated with it 
    :rtype: tuple(bool, int or None)
    """
    pattern = r'goto \((\d+)\)'
    match = re.search(pattern, input_string)
    if match:
        number = int(match.group(1))
        return True, number
    else:
        return False, None