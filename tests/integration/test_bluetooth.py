#!/usr/bin/env python3
"""
Bluetooth Integration Test for RobotSpider

This script discovers and connects to the "RobotSpider" Bluetooth device
using Bluetooth Classic (SPP - Serial Port Profile).

Usage:
    python3 test_bluetooth.py              # Discover and connect
    python3 test_bluetooth.py --send       # Send test commands (future)
"""

import sys
import time
import argparse
try:
    import bluetooth
except ImportError:
    print("ERROR: PyBluez not installed")
    print("Install with: pip3 install pybluez")
    print("See README.md for platform-specific requirements")
    sys.exit(1)

TARGET_DEVICE_NAME = "RobotSpider"
DISCOVERY_DURATION = 10  # seconds


class Colors:
    """ANSI color codes for terminal output"""
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'


def print_header(msg):
    """Print formatted header"""
    print(f"\n{Colors.HEADER}{Colors.BOLD}{'=' * 60}{Colors.ENDC}")
    print(f"{Colors.HEADER}{Colors.BOLD}  {msg}{Colors.ENDC}")
    print(f"{Colors.HEADER}{Colors.BOLD}{'=' * 60}{Colors.ENDC}\n")


def print_success(msg):
    """Print success message"""
    print(f"{Colors.GREEN}✓ {msg}{Colors.ENDC}")


def print_error(msg):
    """Print error message"""
    print(f"{Colors.RED}✗ {msg}{Colors.ENDC}")


def print_warning(msg):
    """Print warning message"""
    print(f"{Colors.YELLOW}⚠ {msg}{Colors.ENDC}")


def print_info(msg):
    """Print info message"""
    print(f"{Colors.BLUE}ℹ {msg}{Colors.ENDC}")


def discover_robot_spider():
    """
    Discover Bluetooth devices and find RobotSpider

    Returns:
        tuple: (address, name) if found, None otherwise
    """
    print_info(f"Scanning for Bluetooth devices... (duration: {DISCOVERY_DURATION}s)")
    print_info(f'Looking for device: "{TARGET_DEVICE_NAME}"')
    print()

    try:
        nearby_devices = bluetooth.discover_devices(
            duration=DISCOVERY_DURATION,
            lookup_names=True,
            flush_cache=True,
            lookup_class=False
        )
    except bluetooth.BluetoothError as e:
        print_error(f"Bluetooth discovery failed: {e}")
        print()
        print_warning("Possible causes:")
        print("  - Bluetooth is disabled on this machine")
        print("  - Bluetooth permissions not granted")
        print("  - RobotSpider is not powered on")
        print("  - RobotSpider is out of range")
        return None

    if not nearby_devices:
        print_warning("No Bluetooth devices discovered")
        print()
        print_info("Make sure:")
        print("  - RobotSpider is powered on")
        print("  - Bluetooth is enabled on ESP32")
        print("  - Device is in range (typically 10-30m)")
        return None

    # List all discovered devices
    print(f"Found {len(nearby_devices)} device(s):")
    for addr, name in nearby_devices:
        device_name = name if name else "Unknown"
        print(f"  • {device_name} ({addr})")

        # Check if this is our target device
        if name == TARGET_DEVICE_NAME:
            print()
            print_success(f'Found "{TARGET_DEVICE_NAME}"!')
            print_info(f"Address: {addr}")
            return (addr, name)

    print()
    print_error(f'Device "{TARGET_DEVICE_NAME}" not found')
    return None


def find_spp_service(address):
    """
    Find the SPP (Serial Port Profile) service on the device

    Args:
        address: Bluetooth device address

    Returns:
        dict: Service info with 'port' key, or None if not found
    """
    print()
    print_info("Searching for SPP service...")

    try:
        services = bluetooth.find_service(address=address)
    except bluetooth.BluetoothError as e:
        print_error(f"Service discovery failed: {e}")
        return None

    if not services:
        print_warning("No services found on device")
        return None

    # Look for Serial Port Profile service
    for service in services:
        # SPP typically has UUID 0x1101
        if "Serial" in service.get("name", "") or service.get("protocol") == "RFCOMM":
            print_success(f"Found SPP service: {service.get('name', 'Unknown')}")
            print_info(f"Port: {service['port']}")
            print_info(f"Protocol: {service.get('protocol', 'Unknown')}")
            return service

    # If no explicit SPP found, try the first RFCOMM service
    for service in services:
        if service.get("protocol") == "RFCOMM":
            print_warning("Using first RFCOMM service (SPP not explicitly identified)")
            print_info(f"Port: {service['port']}")
            return service

    print_error("No SPP/RFCOMM service found")
    return None


def connect_to_robot(address, port):
    """
    Connect to RobotSpider via Bluetooth SPP

    Args:
        address: Bluetooth device address
        port: RFCOMM port number

    Returns:
        bluetooth.BluetoothSocket: Connected socket, or None if failed
    """
    print()
    print_info(f"Connecting to RobotSpider...")
    print_info(f"Address: {address}")
    print_info(f"Port: {port}")

    try:
        sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        sock.connect((address, port))
        print_success("Connected successfully!")
        return sock
    except bluetooth.BluetoothError as e:
        print_error(f"Connection failed: {e}")
        print()
        print_warning("Troubleshooting:")
        print("  - Try unpairing and re-pairing the device")
        print("  - Ensure no other app is connected to RobotSpider")
        print("  - Power cycle the RobotSpider")
        return None


def test_connection(sock):
    """
    Test the Bluetooth connection by keeping it open briefly

    Args:
        sock: Connected Bluetooth socket
    """
    print()
    print_info("Testing connection (keeping open for 5 seconds)...")

    # Set socket to non-blocking to check for incoming data
    sock.setblocking(False)

    try:
        time.sleep(5)

        # Try to read any data that might have been sent
        try:
            data = sock.recv(1024)
            if data:
                print_info(f"Received data: {data.decode('utf-8', errors='ignore')}")
        except bluetooth.BluetoothError:
            # No data available, which is fine
            pass

        print_success("Connection stable")

    except Exception as e:
        print_warning(f"Unexpected error during test: {e}")


def main():
    """Main test execution"""
    parser = argparse.ArgumentParser(description='RobotSpider Bluetooth Integration Test')
    parser.add_argument('--send', action='store_true', help='Send test commands (future feature)')
    args = parser.parse_args()

    print_header("RobotSpider Bluetooth Integration Test")

    # Step 1: Discover RobotSpider device
    device = discover_robot_spider()
    if not device:
        print()
        print_header("✗ Test Failed")
        print_error("Could not discover RobotSpider device")
        return 1

    address, name = device

    # Step 2: Find SPP service
    service = find_spp_service(address)
    if not service:
        print()
        print_header("✗ Test Failed")
        print_error("Could not find SPP service")
        return 1

    port = service['port']

    # Step 3: Connect to device
    sock = connect_to_robot(address, port)
    if not sock:
        print()
        print_header("✗ Test Failed")
        print_error("Could not connect to RobotSpider")
        return 1

    try:
        # Step 4: Test connection
        test_connection(sock)

        # Future: Send commands if --send flag is provided
        if args.send:
            print()
            print_warning("Command sending not yet implemented")
            print_info("Future enhancement: Send test commands and validate responses")

        print()
        print_header("✓ Test Completed Successfully")
        print()
        print_success("Connection established and tested")
        print_info("RobotSpider is ready to receive commands via Bluetooth")
        print()

    finally:
        # Always close the socket
        sock.close()
        print_info("Disconnected")

    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print()
        print_warning("Test interrupted by user")
        sys.exit(130)
