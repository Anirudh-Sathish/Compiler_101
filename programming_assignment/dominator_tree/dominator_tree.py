""" File for dominator tree structure """
from utils import utils
from graphviz import Digraph

class DominatorTreeNode:
    """
    Node structure of the dominator tree
    """
    def __init__(self,block_data = None, block_name = None):
        self.name = block_name
        self.data = block_data
        self.children = []
    def add_child(self,child_node):
        """Adds childern to the node

        :param child_node: child to be added
        :type child_node: DominatorTreeNode
        """
        self.children.append(child_node)

class DominatorTree:
    """
    Dominator Tree Class
    """
    def __init__(self,blocks,idom):
        self.blocks = blocks
        self.idom = idom
        self.dom_nodes = {}
        self.root = DominatorTreeNode()
        for block_num in self.idom:
            self.dom_nodes[block_num] = DominatorTreeNode(self.blocks[block_num],block_num)
    def create_dominator_tree(self):
        """ Traverses blocks of CFG to create dominator tree

        :return: root of dominator tree
        :rtype: DominatorTreeNode
        """
        for block_num, dom in self.idom.items():
            if block_num == 1:
                self.root = self.dom_nodes[block_num]
            if block_num != 1:
                self.dom_nodes[dom].add_child(self.dom_nodes[block_num])
        return self.root
    def traverse_dominator_tree(self):
        """
        Traverses the dominator tree
        """
        visited = set()
        def dfs(node):
            if node is None or node.name in visited:
                return
            visited.add(node.name)
            print("Node:", node.name, "Data:", node.data)
            for child in node.children:
                dfs(child)
        dfs(self.root)
    def view_dominator_tree(self):
        """
        Obtains dot representation of dominator tree 
        """
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
        dfs(self.root)
        final_edges = list(set(edges))
        dot.edges(final_edges)
        dot.render('sample_outputs/dominator_tree', format='png', cleanup=True)
        dot.view()





