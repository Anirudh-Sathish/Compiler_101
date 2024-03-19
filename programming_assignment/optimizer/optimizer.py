# optimizer file 
import re

def get_succesors(cfg):
    block_succesor = {}
    for block in cfg:
        succesors = []
        for line in cfg[block]:
            contains_goto, target = find_goto_with_number(line)
            if contains_goto:
                succesor = find_block(target,cfg)
                succesors.append(str(succesor))
        if block < len(cfg):
            succesors.append(str(block+1))
        block_succesor[block] = succesors
    return block_succesor

def find_block(target,cfg):
    num = 1
    for block in cfg:
        for line in cfg[block]:
            if target == num:
                return block
            num+=1
    return -1
        
def get_cfg(lines):
    leaders = get_leaders(lines)
    blocks = {}
    block_counter = 1
    block = []
    for num in lines:
        if(leaders[num-1] == True):
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
            if ((branch[num-2] == True) and (num !=2)):
                leaders[num-1] = True
    return leaders

def find_goto_with_number(input_string):
    pattern = r'goto \((\d+)\)'
    match = re.search(pattern, input_string)
    if match:
        number = int(match.group(1))
        return True, number
    else:
        return False, None