# main file
import argparse
from utils import utils
from optimizer import optimizer

def main(path):
    text_processor = utils.UtilProcessor(path)
    lines = text_processor.process_text_to_lines()
    for line_no in  lines:
        print(line_no,lines[line_no])
    blocks = optimizer.get_blocks(lines)
    optimizer.add_terminals(blocks)
    adj_list = optimizer.get_graph(blocks)
    dom_relation = optimizer.get_dominance_relation(adj_list,blocks)
    utils.view_adj_as_dot(adj_list,blocks)
    utils.view_dominator_tree(dom_relation,blocks)
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Process text and optimize blocks")
    parser.add_argument("path", type=str, help="Path to the text file")
    args = parser.parse_args()
    main(args.path)