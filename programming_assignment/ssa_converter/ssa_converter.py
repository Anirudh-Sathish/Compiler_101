# SSA converter
import re
import copy
from enum import Enum
from utils import utils
from optimizer import optimizer


class Statement(Enum):
    CONDITIONAL = 1
    ASSIGNMENT = 2

class SSA_Converter:
    def __init__(self,adj_list,blocks,df):
        self.adj_list = adj_list
        self.blocks = blocks
        self.df = df
        self.phi_symbol = "\u03C6"
    def get_variables_in_block(self):
        block_variables  = {}
        variables = set()
        for block_num in self.blocks:
            block_vars = []
            if (block_num == 0) or (block_num == len(self.blocks)-1):
                continue
            else:
                for line in self.blocks[block_num]:
                    vars = utils.extract_variables(line)
                    block_vars+=vars
            block_variables[block_num] = list(set(block_vars))
            variables.update(set(block_vars))
        return variables,block_variables
    def convert_cfg_ssa(self):
        variables, block_variables = self.get_variables_in_block()
        self.__insert_phi__(variables,block_variables)
        count = {}
        stack = {}
        visited = set()
        for var in variables:
            count[var] = 0
            stack[var] = []
            stack[var].append(0)
        self.rename(1,variables,stack,count,block_variables,visited)
        return self.blocks
    def rename(self,block_num,variables,stack,count,block_variables,visited):
        block_copy = copy.deepcopy(self.blocks[block_num])
        for i, stmt in enumerate(self.blocks[block_num]):
            stmt_type = self.classify_statement(stmt)
            if stmt_type.value == 1:
                stmt_vars = [v for v in stmt.split(" ") if v in variables]
            else:
                defn, rest_part = [part.strip() for part in stmt.split("=", 1)]
                stmt_vars = [v for v in rest_part.split(" ") if v in variables]
            if self.phi_symbol not in stmt:
                for block_variable in block_variables[block_num]:
                    for stmt_var in stmt_vars:
                        if block_variable == stmt_var:
                            original_var_index = stack[stmt_var][-1]
                            new_var = stmt_var + str(original_var_index)
                            if stmt_type.value == 1:
                                self.blocks[block_num][i] = re.sub(r'\b' + re.escape(stmt_var) + r'\b', new_var, stmt)+"\n"
                            else:
                                self.blocks[block_num][i] = defn + " = " + rest_part.replace(stmt_var, new_var, 1)+"\n"
            if stmt_type.value ==2:
                for block_variable in variables:
                    defn, rest_part = [part.strip() for part in self.blocks[block_num][i].split("=", 1)]
                    if block_variable == defn:
                        count[defn]+=1
                        var_index = count[defn]
                        stack[defn].append(var_index)
                        new_defn = defn + str(var_index)
                        self.blocks[block_num][i] = new_defn + " = " + rest_part+"\n"
        for next_block in self.adj_list[block_num]:
            if next_block == (len(self.blocks)-1):
                break
            preds = optimizer.find_predecessors(next_block,self.adj_list)
            route_index = preds.index(block_num)
            for i, stmt in enumerate(self.blocks[next_block]):
                if self.phi_symbol in stmt:
                    inside_phi = self.extract_elements_with_phi(stmt)[0].split(",")
                    inside_phi = [element.strip() for element in inside_phi]
                    var_phi = inside_phi[route_index]
                    if var_phi in variables:
                        var_index = stack[var_phi][-1]
                        inside_phi[route_index] += str(var_index)
                        updated_stmt = stmt.replace(stmt[stmt.find("(")+1:stmt.find(")")], ','.join(inside_phi), 1)
                        self.blocks[next_block][i] = updated_stmt
        for next_block in self.adj_list[block_num]:
            if next_block == (len(self.blocks)-1):
                continue
            if next_block in visited:
                continue
            visited.add(next_block)
            self.rename(next_block,variables,stack,count,block_variables,visited)
        for i, stmt in enumerate(block_copy):
            stmt_type = self.classify_statement(stmt)
            if stmt_type.value == 2:
                defn, rest_part = [part.strip() for part in stmt.split("=", 1)]
                if defn in variables:
                    _ = stack[defn].pop()

    def classify_statement(self,stmt):
        words = stmt.split(" ")
        if words[0] == 'if':
            return Statement.CONDITIONAL
        return Statement.ASSIGNMENT
    def extract_elements_with_phi(self,stmt):
        pattern = r'\u03C6\s*\((.*?)\)'
        matches = re.findall(pattern, stmt)
        return matches
    def find_words_before_equal(self,line):
        pattern = r'\b(\w+)\s*='
        matches = re.findall(pattern, line)
        return matches

    def __insert_phi__(self,variables,block_variables):
        def_sites = {}
        variables_phi = {}
        for v in variables:
            def_sites[v] = set()
        for num in self.blocks:
            variables_phi[num] = set()
        for num, _ in self.blocks.items():
            if num ==0 or (num == len(self.blocks)-1):
                continue
            else:
                for var in block_variables[num]:
                    def_sites[var].update({num})
        for var in variables:
            sites = def_sites[var]
            while(len(sites)!= 0):
                n = sites.pop()
                for Y in self.df[n]:
                    if Y not in variables_phi[n]:
                        predecessors = optimizer.find_predecessors(Y,self.adj_list)
                        repeated_var = ', '.join([var] * len(predecessors))
                        phi_line = [f"{var} = {self.phi_symbol} ( {repeated_var} )"]
                        self.blocks[Y] = phi_line + self.blocks[Y]
                        variables_phi[n].update({Y})
                        if Y not in block_variables[n]:
                            sites.update({Y})