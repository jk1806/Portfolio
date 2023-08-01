#!/usr/bin/env python3
"""
Ghidra script for firmware analysis
Run in Ghidra Script Manager
"""

from ghidra.program.model.listing import FunctionManager
from ghidra.program.model.symbol import SymbolTable

def analyze_firmware():
    """Analyze firmware in Ghidra"""
    print("=== Firmware Analysis ===")
    
    # Get all functions
    func_manager = currentProgram.getFunctionManager()
    functions = func_manager.getFunctions(True)
    
    print(f"Total functions: {functions.size()}")
    
    # Analyze each function
    for func in functions:
        name = func.getName()
        entry = func.getEntryPoint()
        body = func.getBody()
        
        print(f"Function: {name} at {entry}")
        print(f"  Size: {body.getNumAddresses()} bytes")
        
        # Look for crypto functions
        if 'crypto' in name.lower() or 'encrypt' in name.lower():
            print(f"  [CRYPTO] Potential cryptographic function")
        
        # Look for network functions
        if 'socket' in name.lower() or 'network' in name.lower():
            print(f"  [NETWORK] Potential network function")

if __name__ == '__main__':
    analyze_firmware()

