import dataclasses
import enum
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable, Dict, Iterable, List, Optional, Union


@dataclass
class YulChildren(object):
    """An Iterable wrapper around the underlying list that returns YulNodes"""

    underlying: List[dict]

    def __iter__(self):
        return (YulNode(obj) for obj in self.underlying)

    def __getitem__(self, i: int):
        return YulNode(self.underlying[i])

    def __setitem__(self, i: int, x: Union["YulNode", dict]):
        assert isinstance(x, (YulNode, dict))
        self.underlying[i] = x.obj if isinstance(x, YulNode) else x

    def __len__(self):
        return self.underlying.__len__()


@dataclass
class YulNode(object):
    """A wrapper around the Yul AST node dictionary that provides convenience
    methods."""

    obj: dict

    @property
    def type(self) -> str:
        return self.obj["type"]

    @type.setter
    def type(self, ty: str):
        self.obj["type"] = ty

    @property
    def children(self) -> YulChildren:
        return YulChildren(self.obj["children"])

    def is_fun_def(self) -> bool:
        return self.type == "yul_function_definition"

    def is_block(self) -> bool:
        return self.type == "yul_block"

    def is_fun_call(self) -> bool:
        return self.type == "yul_function_call"

    def is_var_decl(self) -> bool:
        return self.type == "yul_variable_declaration"

    def get_decl_var_name(self) -> str:
        assert self.type == "yul_variable_declaration"
        child = self.children[0]
        assert child.type == "yul_typed_identifier_list"
        child = child.children[0]
        assert child.type == "yul_identifier"
        return child.children[0].obj

    def is_var_decl_with_name(self, name) -> bool:
        return self.is_var_decl() and self.get_decl_var_name() == name

    def is_fun_call_and_name(self, name) -> bool:
        return self.is_fun_call() and self.get_fun_name() == name

    def set_fun_call_name(self, name):
        assert self.type == "yul_function_call"
        self.children.obj[0] = name

    def get_yul_identifier_node(self, id: str):
        return create_yul_node("yul_identifier", [id])

    def get_fun_name(self) -> str:
        # FIXME: this should be part of the object, not its child.
        # First child is yul identifier node containing the name
        return self.obj["children"][0]["children"][0]

    def get_literal_value(self) -> Union[str, int]:
        """Returns the value of a yul_literal node."""
        assert self.type == "yul_literal"
        lit_node = self.obj["children"][0]
        # FIXME: yul_parser should generate a flat literal node.
        if lit_node["type"] == "yul_number_literal":
            n_node = lit_node["children"][0]
            if n_node["type"] == "yul_hex_number":
                return int(n_node["children"][0], base=16)
            elif n_node["type"] == "yul_dec_number":
                return int(n_node["children"][0])
        raise NotImplementedError(f"unknown literal node: {lit_node}")

    def get_identifier(self) -> str:
        """Returns the identifier value of yul_identifier node."""
        assert self.type == "yul_identifier"
        return self.obj["children"][0]


def walk_dfs(
    root: Union[YulNode, dict],
    callback: Callable[[YulNode, List[YulNode]], Optional[bool]],
):
    """Walk the AST in depth-first order.

    :param callback: Callback to invoke on each node. Returns whether the walk
    should recurse into the children.
    """
    parents: List[YulNode] = []
    if isinstance(root, dict):
        root = YulNode(root)

    # Sentinal marks the stack in to_visit nodes
    sentinal = YulNode({"type": "sentinal", "children": []})
    to_visit = [root]
    while to_visit:
        # Pass parents to the callback
        obj = to_visit.pop()
        if obj == sentinal:
            if len(parents) > 0:
                parents.pop()
                continue
        if len(obj.children) > 0:
            parents.append(obj)
            to_visit.append(sentinal)

        should_recurse = callback(obj, parents)
        if should_recurse is None or should_recurse:
            for child in obj.children:
                # FIXME: don't add the literal values as children...
                if isinstance(child.obj, dict):
                    to_visit.append(child)


def create_yul_node(
    type: str, children: Iterable[Union[dict, YulNode, str]]
) -> YulNode:
    return YulNode(
        {
            "type": type,
            "children": [c.obj if isinstance(c, YulNode) else c for c in children],
        }
    )


def create_yul_fun_call(
    fname: str, args: Iterable[Union[dict, YulNode, str]]
) -> YulNode:
    arg_nodes = [create_yul_identifier(fname)]
    arg_nodes.extend(args)
    return create_yul_node("yul_function_call", arg_nodes)


def create_yul_identifier(name: str) -> YulNode:
    return create_yul_node("yul_identifier", [name])


def create_yul_string_literal(name: str) -> YulNode:
    return create_yul_node(
        "yul_literal", [create_yul_node("yul_string_literal", [name])]
    )


def create_yul_number_literal(num: int) -> YulNode:
    return create_yul_node(
        "yul_literal",
        [
            create_yul_node(
                "yul_number_literal", [create_yul_node("yul_dec_number", [str(num)])]
            )
        ],
    )


class YulTypeKind(str, enum.Enum):
    INT_LIKE = "intlike"
    MAPPING = "mapping"
    STRUCT = "struct"
    ARRAY = "array"
    BYTES = "bytes"


@dataclass
class YulType(object):
    pretty_name: str
    kind: YulTypeKind = field(kw_only=True, init=False)


@dataclass
class YulIntType(YulType):
    _: dataclasses.KW_ONLY
    size: int
    kind: YulTypeKind = YulTypeKind.INT_LIKE


@dataclass
class YulStructType(YulType):
    _: dataclasses.KW_ONLY
    fields: Dict[str, "YulStateVar"]
    kind: YulTypeKind = YulTypeKind.STRUCT


@dataclass
class YulMappingType(YulType):
    _: dataclasses.KW_ONLY
    key: str
    value: str
    kind: YulTypeKind = YulTypeKind.MAPPING


@dataclass
class YulBytesType(YulType):
    size: int
    _: dataclasses.KW_ONLY
    kind: YulTypeKind = YulTypeKind.BYTES


@dataclass
class YulStateVar(object):
    name: str
    type: str
    offset: int
    slot: int


@dataclass
class YulMetadata(object):
    main_ctor: str = ""
    external_fns: Dict[str, str] = field(default_factory=dict)
    state_vars: List[YulStateVar] = field(default_factory=list)
    types: Dict[str, YulType] = field(default_factory=dict)


@dataclass
class ContractData(object):
    """solc output for one contract"""

    name: str
    abi: List[dict]
    storageLayout: dict
    yul_text: str
    out_dir: Path
    yul_ast: dict = field(default_factory=dict)
    metadata: YulMetadata = field(default_factory=YulMetadata)
