#!/usr/bin/env python3
"""Generate comprehensive security testing report"""

import json
import os
from datetime import datetime

def collect_results():
    """Collect results from all tests"""
    results = {
        'timestamp': datetime.now().isoformat(),
        'static_analysis': {},
        'dynamic_analysis': {},
        'fuzzing': {},
        'pentesting': {}
    }
    
    # Static analysis results
    if os.path.exists('reports/cppcheck/report.txt'):
        with open('reports/cppcheck/report.txt', 'r') as f:
            results['static_analysis']['cppcheck'] = f.read()[:1000]  # First 1000 chars
    
    # Dynamic analysis results
    if os.path.exists('valgrind.log'):
        with open('valgrind.log', 'r') as f:
            results['dynamic_analysis']['valgrind'] = f.read()[:1000]
    
    # Fuzzing results
    if os.path.exists('afl_output'):
        results['fuzzing']['afl'] = 'Fuzzing results available in afl_output/'
    
    return results

def generate_html_report(results):
    """Generate HTML report"""
    html = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Security Testing Report</title>
        <style>
            body {{ font-family: Arial, sans-serif; margin: 20px; }}
            h1 {{ color: #333; }}
            .section {{ margin: 20px 0; padding: 10px; border: 1px solid #ddd; }}
        </style>
    </head>
    <body>
        <h1>Security Testing Report</h1>
        <p>Generated: {results['timestamp']}</p>
        
        <div class="section">
            <h2>Static Analysis</h2>
            <pre>{results.get('static_analysis', {}).get('cppcheck', 'No results')}</pre>
        </div>
        
        <div class="section">
            <h2>Dynamic Analysis</h2>
            <pre>{results.get('dynamic_analysis', {}).get('valgrind', 'No results')}</pre>
        </div>
        
        <div class="section">
            <h2>Fuzzing</h2>
            <pre>{results.get('fuzzing', {}).get('afl', 'No results')}</pre>
        </div>
    </body>
    </html>
    """
    
    with open('reports/security_report.html', 'w') as f:
        f.write(html)
    
    print("HTML report generated: reports/security_report.html")

def main():
    results = collect_results()
    
    # JSON report
    with open('reports/security_report.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    # HTML report
    generate_html_report(results)
    
    print("Reports generated successfully")

if __name__ == '__main__':
    main()

