#!/usr/bin/env python3
"""
AI-Driven Thermal Management System
Author: jk1806
Created: 2024-01-15
"""

import numpy as np
import time
import threading
from collections import deque
from typing import List, Dict, Optional

class ThermalSensor:
    """Thermal sensor with AI prediction capabilities"""
    
    def __init__(self, sensor_id: str, location: str):
        self.sensor_id = sensor_id
        self.location = location
        self.current_temp = 25.0  # Default room temperature
        self.temp_history = deque(maxlen=100)  # Keep last 100 readings
        self.predicted_temp = 25.0
        # Personal debugging: Added after prediction accuracy issues
        self.debug_counters = [0, 0, 0, 0]
        self.prediction_errors = []
    
    def read_temperature(self) -> float:
        """Read current temperature with some realistic variation"""
        # Simulate temperature reading with noise
        noise = np.random.normal(0, 0.5)
        self.current_temp += noise
        self.current_temp = max(20.0, min(85.0, self.current_temp))  # Clamp to realistic range
        
        self.temp_history.append(self.current_temp)
        self.debug_counters[0] += 1  # Read counter
        
        return self.current_temp
    
    def predict_temperature(self, steps_ahead: int = 5) -> float:
        """Predict temperature using simple linear regression"""
        if len(self.temp_history) < 10:
            return self.current_temp
        
        # Simple trend prediction
        recent_temps = list(self.temp_history)[-10:]
        trend = np.polyfit(range(len(recent_temps)), recent_temps, 1)[0]
        
        self.predicted_temp = self.current_temp + (trend * steps_ahead)
        self.debug_counters[1] += 1  # Prediction counter
        
        # Track prediction accuracy
        if len(self.temp_history) > 5:
            actual = self.temp_history[-1]
            predicted = self.predicted_temp
            error = abs(actual - predicted)
            self.prediction_errors.append(error)
            if len(self.prediction_errors) > 50:
                self.prediction_errors.pop(0)
        
        return self.predicted_temp

class FanController:
    """Intelligent fan controller with AI optimization"""
    
    def __init__(self, fan_id: str):
        self.fan_id = fan_id
        self.current_speed = 0.0  # 0-100%
        self.target_speed = 0.0
        self.is_active = False
        # TODO: Add support for PWM control
        self.pwm_duty_cycle = 0
        # Personal debugging: Added after speed control issues
        self.speed_adjustments = 0
        self.overshoot_count = 0
    
    def set_speed(self, speed: float) -> bool:
        """Set fan speed with smooth transitions"""
        if speed < 0.0 or speed > 100.0:
            print(f"ERROR: Invalid fan speed {speed}%")
            return False
        
        self.target_speed = speed
        
        # Smooth speed transition
        if abs(self.current_speed - self.target_speed) > 5.0:
            self.speed_adjustments += 1
            print(f"Fan {self.fan_id}: Adjusting speed {self.current_speed:.1f}% -> {self.target_speed:.1f}%")
        
        self.current_speed = self.target_speed
        self.is_active = self.current_speed > 0.0
        
        return True
    
    def get_status(self) -> Dict:
        """Get fan status information"""
        return {
            'fan_id': self.fan_id,
            'current_speed': self.current_speed,
            'target_speed': self.target_speed,
            'is_active': self.is_active,
            'speed_adjustments': self.speed_adjustments
        }

class ThermalManagementAI:
    """AI-driven thermal management system"""
    
    def __init__(self):
        self.sensors: List[ThermalSensor] = []
        self.fans: List[FanController] = []
        self.is_running = False
        self.control_thread = None
        # Personal debugging: Added after thermal runaway issues
        self.thermal_events = 0
        self.emergency_shutdowns = 0
        self.debug_flags = 0
        
        # AI model parameters (simplified)
        self.temp_thresholds = {
            'warning': 70.0,
            'critical': 80.0,
            'emergency': 85.0
        }
        
        self.fan_curves = {
            'low': (50.0, 30.0),    # temp, speed
            'medium': (65.0, 60.0),
            'high': (75.0, 90.0)
        }
    
    def add_sensor(self, sensor_id: str, location: str) -> ThermalSensor:
        """Add thermal sensor to the system"""
        sensor = ThermalSensor(sensor_id, location)
        self.sensors.append(sensor)
        print(f"Added thermal sensor: {sensor_id} at {location}")
        return sensor
    
    def add_fan(self, fan_id: str) -> FanController:
        """Add fan controller to the system"""
        fan = FanController(fan_id)
        self.fans.append(fan)
        print(f"Added fan controller: {fan_id}")
        return fan
    
    def get_max_temperature(self) -> float:
        """Get maximum temperature across all sensors"""
        if not self.sensors:
            return 0.0
        
        max_temp = max(sensor.current_temp for sensor in self.sensors)
        return max_temp
    
    def calculate_fan_speed(self, temperature: float) -> float:
        """Calculate optimal fan speed based on temperature"""
        if temperature < self.temp_thresholds['warning']:
            return 0.0  # No cooling needed
        
        # Linear interpolation between thresholds
        if temperature < self.temp_thresholds['critical']:
            # Warning to critical range
            ratio = (temperature - self.temp_thresholds['warning']) / \
                   (self.temp_thresholds['critical'] - self.temp_thresholds['warning'])
            return 30.0 + (ratio * 30.0)  # 30% to 60%
        
        elif temperature < self.temp_thresholds['emergency']:
            # Critical to emergency range
            ratio = (temperature - self.temp_thresholds['critical']) / \
                   (self.temp_thresholds['emergency'] - self.temp_thresholds['critical'])
            return 60.0 + (ratio * 30.0)  # 60% to 90%
        
        else:
            # Emergency - maximum speed
            return 100.0
    
    def control_loop(self):
        """Main AI control loop"""
        print("Starting thermal management AI control loop...")
        
        while self.is_running:
            try:
                # Read all sensors
                for sensor in self.sensors:
                    sensor.read_temperature()
                
                # Get maximum temperature
                max_temp = self.get_max_temperature()
                
                # Calculate fan speed
                fan_speed = self.calculate_fan_speed(max_temp)
                
                # Update all fans
                for fan in self.fans:
                    fan.set_speed(fan_speed)
                
                # Check for thermal events
                if max_temp > self.temp_thresholds['critical']:
                    self.thermal_events += 1
                    print(f"WARNING: High temperature detected: {max_temp:.1f}°C")
                
                if max_temp > self.temp_thresholds['emergency']:
                    self.emergency_shutdowns += 1
                    print(f"EMERGENCY: Critical temperature: {max_temp:.1f}°C - Shutting down!")
                    self.stop()
                    break
                
                # Sleep for control interval
                time.sleep(1.0)  # 1 second control interval
                
            except Exception as e:
                print(f"ERROR in control loop: {e}")
                time.sleep(1.0)
    
    def start(self):
        """Start the thermal management system"""
        if self.is_running:
            print("Thermal management system already running")
            return
        
        self.is_running = True
        self.control_thread = threading.Thread(target=self.control_loop)
        self.control_thread.daemon = True
        self.control_thread.start()
        print("Thermal management AI started")
    
    def stop(self):
        """Stop the thermal management system"""
        self.is_running = False
        if self.control_thread:
            self.control_thread.join(timeout=2.0)
        print("Thermal management AI stopped")
    
    def get_status(self) -> Dict:
        """Get system status"""
        status = {
            'is_running': self.is_running,
            'sensors': len(self.sensors),
            'fans': len(self.fans),
            'max_temperature': self.get_max_temperature(),
            'thermal_events': self.thermal_events,
            'emergency_shutdowns': self.emergency_shutdowns
        }
        
        # Add sensor details
        status['sensor_data'] = []
        for sensor in self.sensors:
            status['sensor_data'].append({
                'sensor_id': sensor.sensor_id,
                'location': sensor.location,
                'current_temp': sensor.current_temp,
                'predicted_temp': sensor.predicted_temp
            })
        
        # Add fan details
        status['fan_data'] = []
        for fan in self.fans:
            status['fan_data'].append(fan.get_status())
        
        return status

def main():
    """Main function to demonstrate thermal management AI"""
    print("=== AI-Driven Thermal Management System ===")
    
    # Create thermal management system
    thermal_ai = ThermalManagementAI()
    
    # Add sensors
    thermal_ai.add_sensor("CPU_TEMP", "CPU Package")
    thermal_ai.add_sensor("GPU_TEMP", "GPU Core")
    thermal_ai.add_sensor("SYS_TEMP", "System Board")
    
    # Add fans
    thermal_ai.add_fan("CPU_FAN")
    thermal_ai.add_fan("CASE_FAN_1")
    thermal_ai.add_fan("CASE_FAN_2")
    
    # Start the system
    thermal_ai.start()
    
    try:
        # Run for demonstration
        for i in range(30):  # Run for 30 seconds
            time.sleep(1)
            if i % 5 == 0:  # Print status every 5 seconds
                status = thermal_ai.get_status()
                print(f"Status: Max temp={status['max_temperature']:.1f}°C, "
                      f"Fans={status['fans']}, Events={status['thermal_events']}")
    
    except KeyboardInterrupt:
        print("\nShutting down thermal management system...")
    
    finally:
        thermal_ai.stop()
        print("Thermal management system shutdown complete")

if __name__ == "__main__":
    main()
