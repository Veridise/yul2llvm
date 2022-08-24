from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable, Dict, Iterable, List, Optional, Union


@dataclass
class YulChildren(object):
    '''An Iterable wrapper around the underlying list that returns YulNodes'''
    underlying: List[dict]

    def __iter__(self):
        return (YulNode(obj) for obj in self.underlying)

    def __getitem__(self, i: int):
        return YulNode(self.underlying[i])

    def __setitem__(self, i: int, x: Union['YulNode', dict]):
        assert isinstance(x, (YulNode, dict))
        self.underlying[i] = x.obj if isinstance(x, YulNode) else x

    def __len__(self):
        return self.underlying.__len__()


@dataclass
class YulNode(object):
    '''A wrapper around the Yul AST node dictionary that provides convenience
    methods.'''

    obj: dict

    @property
    def type(self) -> str:
        return self.obj['type']

    @type.setter
    def type(self, ty: str):
        self.obj['type'] = ty

    @property
    def children(self) -> YulChildren:
        return YulChildren(self.obj['children'])

    def is_fun_def(self) -> bool:
        return self.type == 'yul_function_definition'

    def is_fun_call(self) -> bool:
        return self.type == 'yul_function_call'

    def get_fun_name(self) -> str:
        # FIXME: this should be part of the object, not its child.
        # First child is yul identifier node containing the name
        return self.obj['children'][0]['children'][0]


def walk_dfs(root: Union[YulNode, dict], callback: Callable[[YulNode], Optional[bool]]):
    '''Walk the AST in depth-first order.

    :param callback: Callback to invoke on each node. Returns whether the walk
    should recurse into the children.
    '''

    if isinstance(root, dict):
        root = YulNode(root)

    to_visit = [root]
    while to_visit:
        obj = to_visit.pop()
        should_recurse = callback(obj)

        if should_recurse is None or should_recurse:
            for child in obj.children:
                # FIXME: don't add the literal values as children...
                if isinstance(child.obj, dict):
                    to_visit.append(child)


def create_yul_node(type: str,
                    children: Iterable[Union[dict, YulNode, str]]) -> YulNode:
    return YulNode({
        'type': type,
        'children': [c.obj if isinstance(c, YulNode) else c for c in children]
    })


@dataclass
class YulMetadata(object):
    main_ctor: str = ''
    external_fns: Dict[str, str] = field(default_factory=dict)


@dataclass
class ContractData(object):
    '''solc output for one contract'''
    name: str
    abi: List[dict]
    storageLayout: List[dict]
    yul_text: str
    out_dir: Path
    yul_ast: dict = field(default_factory=dict)
    metadata: YulMetadata = field(default_factory=YulMetadata)

