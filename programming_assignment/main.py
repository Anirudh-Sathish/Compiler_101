# main file
from utils import utils
from optimizer import optimizer
def main():
    path = "test.txt"
    text_processor = utils.UtilProcessor(path)
    lines = text_processor.process_text_to_lines()
    for line_no in  lines:
        print(line_no,lines[line_no])
    blocks = optimizer.get_blocks(lines)
    optimizer.add_terminals(blocks)
    adj_list = optimizer.get_graph(blocks)
    utils.view_adj_as_dot(adj_list,blocks)
    # utils.view_cfg_as_dot(adj_list)
    


if __name__ == '__main__':
    main()