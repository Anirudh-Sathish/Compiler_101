# Utils file
from graphviz import Digraph

class UtilProcessor:
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
        lines = self.__assign_line_number__(self.__read_text_to_lines__())
        return lines
def view_cfg_as_dot(cfg,succesors):
    dot = Digraph()
    dot.node('0','ENTRY')
    edges = []
    for block in cfg:
        if block ==1:
            edges.append(str(block-1)+str(block))
        block_content = ''.join(cfg[block])
        dot.node(str(block),block_content)
        for n in succesors[block]:
            edge = str(block)+n
            edges.append(edge)
    dot.node(str(len(cfg)+1),"EXIT")
    edges.append(str(len(cfg))+str(len(cfg)+1))
    dot.edges(edges)
    dot.render('graph', format='png', cleanup=True)
    dot.view()