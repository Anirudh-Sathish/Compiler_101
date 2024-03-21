# main file
import argparse
from utils import utils
from optimizer import optimizer
from dominator_tree import dominator_tree

def main(path):
    text_processor = utils.UtilProcessor(path)
    lines = text_processor.process_text_to_lines()
    for line_no in  lines:
        print(line_no,lines[line_no])
    blocks = optimizer.get_blocks(lines)
    optimizer.add_terminals(blocks)
    adj_list = optimizer.get_graph(blocks)
    dom_relation = optimizer.get_dominance_relation(adj_list,blocks)
    dom_tree = dominator_tree.DominatorTree(dom_relation,blocks)
    dominator_root = dom_tree.create_dominator_tree()
    dom_tree.traverse_dominator_tree()
    dom_tree.view_dominator_tree()
    utils.view_adj_as_dot(adj_list,blocks)
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Process text and optimize blocks")
    parser.add_argument("path", type=str, help="Path to the text file")
    args = parser.parse_args()
    main(args.path)