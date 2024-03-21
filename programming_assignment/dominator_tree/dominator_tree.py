""" File for dominator tree structure """
from utils import utils
from graphviz import Digraph

class DominatorTreeNode:
    def __init__(self,block_data,block_name):
        self.name = block_name
        self.data = block_data
        self.children = []
    def add_child(self,child_node):
        self.children.append(child_node)

class DominatorTree:
    def __init__(self,dom_relation,blocks):
        self.dom_relation = dom_relation
        self.blocks = blocks
        self.dom_nodes = {}
    def __create_dominator_nodes__(self,):
        for block_num in self.dom_relation:
            self.dom_nodes[block_num] = DominatorTreeNode(self.blocks[block_num],block_num)
    def create_dominator_tree(self):
        self.__create_dominator_nodes__()
        for block_num, doms in self.dom_relation.items():
            if block_num != 1:
                second_max = utils.find_second_maximum(doms)
                self.dom_nodes[second_max].add_child(self.dom_nodes[block_num])        
    def traverse_dominator_tree(self):
        visited = set()
        def dfs(node):
            if node is None or node.name in visited:
                return
            visited.add(node.name)
            print("Node:", node.name, "Data:", node.data)
            for child in node.children:
                dfs(child)
        dfs(self.dom_nodes[1])

    def view_dominator_tree(self):
        dot = Digraph()
        visited = set()
        edges = []

        def dfs(node):
            if node is None or node.name in visited:
                return
            visited.add(node.name)
            block_content = ''.join(node.data)
            dot.node(str(node.name),block_content)
            for child in node.children:
                edge = str(node.name) + str(child.name)
                edges.append(edge)
                dfs(child)

        dfs(self.dom_nodes[1])
        final_edges = list(set(edges))
        dot.edges(final_edges)
        dot.render('sample_outputs/dominator_tree', format='png', cleanup=True)
        dot.view()




