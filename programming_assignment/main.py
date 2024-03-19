# main file
from utils import utils
from optimizer import optimizer
def main():
    path = "test.txt"
    text_processor = utils.UtilProcessor(path)
    lines = text_processor.process_text_to_lines()
    for line_no in  lines:
        print(line_no,lines[line_no])
    cfg = optimizer.get_cfg(lines)
    succesors = optimizer.get_succesors(cfg)
    utils.view_cfg_as_dot(cfg,succesors)
    


if __name__ == '__main__':
    main()