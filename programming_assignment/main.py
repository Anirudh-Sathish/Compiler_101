# main file
import argparse
from utils import utils
from optimizer import optimizer
from dominator_tree import dominator_tree
from ssa_converter import ssa_converter

def main(path):
    text_processor = utils.UtilProcessor(path)
    lines = text_processor.process_text_to_lines()
    blocks = optimizer.get_blocks(lines)
    optimizer.add_terminals(blocks)
    adj_list = optimizer.get_graph(blocks)
    dom_relation = optimizer.get_dominance_relation(adj_list,blocks)
    idom = optimizer.get_idom(dom_relation)
    dom_tree = dominator_tree.DominatorTree(blocks,idom)
    dominator_root = dom_tree.create_dominator_tree()
    dom_tree.view_dominator_tree()
    utils.view_adj_as_dot(adj_list,blocks,"cfg")
    dom_front = {}
    df = optimizer.compute_dominance_frontier(dominator_root,idom,adj_list,dom_front,dom_relation)
    ssa_convert = ssa_converter.SSAconverter(adj_list,blocks,df)
    updated_blocks = ssa_convert.convert_cfg_ssa()
    utils.view_adj_as_dot(adj_list,updated_blocks,"ssa")
 
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Process text and optimize blocks")
    parser.add_argument("path", type=str, help="Path to the text file")
    args = parser.parse_args()
    main(args.path)