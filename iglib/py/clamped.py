from dataclasses import dataclass
from decimal import Decimal, Context
import math

@dataclass(slots=True, frozen=True)
class Quadruple:
	top: int # 1-sign and 23-expo
	bottom: int # 
