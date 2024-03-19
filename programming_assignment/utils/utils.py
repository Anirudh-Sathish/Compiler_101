# Utils file
from graphviz import Digraph

class UtilProcessor:
    """
    Utility class to process input three address code
    """
    def __init__(self,path):
        self.path = path
    def __read_text_to_lines__(self):
        with open(self.path,'r') as file:
            lines = file.readlines()
        return lines
    def __assign_line_number__(self,lines):
        updated_lines = {}
        for i,line in enumerate(lines):
            updated_lines[i+1] = line
        return updated_lines
    def process_text_to_lines(self):
        """Converts text file into strings of three adress code

        :return: dictionary containing line numbers for corresponding lines
        :rtype: dict
        """
        lines = self.__assign_line_number__(self.__read_text_to_lines__())
        return lines

def view_dominator_tree(dom_relation,blocks):
    """Gives dot representation of dominator tree given the domination relation and
    the blocks

    :param dom_relation: domination relation of the blocks
    :type dom_relation: dict
    :param blocks: blocks of the cfg
    :type blocks: dict
    """
    dot = Digraph()
    edges = []
    for block_num in dom_relation:
        if block_num ==1:
            block_content = ''.join(blocks[block_num])
            dot.node(str(block_num),block_content)
        else:
            block_content = ''.join(blocks[block_num])
            dot.node(str(block_num),block_content)
            second_max = find_second_maximum(dom_relation[block_num])
            edge = str(second_max)+str(block_num)
            edges.append(edge)
    final_edges = list(set(edges))
    dot.edges(final_edges)
    dot.render('dominator_tree', format='png', cleanup=True)
    dot.view()

def view_adj_as_dot(adj,blocks):
    """Gives the dot representation of the cfg

    :param adj: Adjacency list representation of the CFG
    :type adj: dict
    :param blocks: Block representation of the CFG
    :type blocks: dict
    """
    dot = Digraph()
    edges = []
    for block in adj:
        block_content = ''.join(blocks[block])
        dot.node(str(block),block_content)
        for ngbr in adj[block]:
            ngbr_content = ''.join(adj[block][ngbr])
            dot.node(str(ngbr),ngbr_content)
            edge = str(block)+str(ngbr)
            edges.append(edge)
    final_edges = list(set(edges))
    dot.edges(final_edges)
    dot.render('cfg', format='png', cleanup=True)
    dot.view()


def find_second_maximum(lst):
    """Gives the second largest element in a list

    :param lst: Given a list
    :type lst: list
    :return: second largest element in the list
    :rtype: int
    """
    if len(lst) < 2:
        return "List should have at least two elements"
    sorted_list = sorted(lst, reverse=True)
    return sorted_list[1]
