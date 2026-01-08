#!/usr/bin/env node

/**
 * Bluetooth Integration Test for RobotSpider
 *
 * This script discovers and connects to the "RobotSpider" Bluetooth device
 * using Bluetooth Classic (SPP - Serial Port Profile).
 *
 * Usage:
 *   npm install              # Install dependencies
 *   npm test                 # Discover and pair with RobotSpider
 *   npm run send             # Send test commands (future)
 */

import btSerial from 'bluetooth-serial-port';

const TARGET_DEVICE_NAME = 'RobotSpider';
const DISCOVERY_TIMEOUT_MS = 10000;

/**
 * Discover Bluetooth devices and find RobotSpider
 */
async function discoverRobotSpider() {
  console.log('🔍 Scanning for Bluetooth devices...');
  console.log(`   Looking for device: "${TARGET_DEVICE_NAME}"`);
  console.log('');

  const btSerialPort = new btSerial.BluetoothSerialPort();

  return new Promise((resolve, reject) => {
    let deviceFound = false;
    const discoveredDevices = [];

    // Timeout for discovery
    const timeout = setTimeout(() => {
      if (!deviceFound) {
        console.log('\n⏱️  Discovery timeout');
        console.log(`\n❌ Device "${TARGET_DEVICE_NAME}" not found`);

        if (discoveredDevices.length > 0) {
          console.log('\n📱 Discovered devices:');
          discoveredDevices.forEach((dev, idx) => {
            console.log(`   ${idx + 1}. ${dev.name} (${dev.address})`);
          });
        } else {
          console.log('\n💡 No Bluetooth devices discovered');
          console.log('   Make sure:');
          console.log('   - RobotSpider is powered on');
          console.log('   - Bluetooth is enabled on ESP32');
          console.log('   - Device is in range');
        }

        reject(new Error('Device not found'));
      }
    }, DISCOVERY_TIMEOUT_MS);

    // Listen for discovered devices
    btSerialPort.on('found', function(address, name) {
      const device = { address, name };
      discoveredDevices.push(device);

      console.log(`   Found: ${name || 'Unknown'} (${address})`);

      // Check if this is our target device
      if (name === TARGET_DEVICE_NAME) {
        deviceFound = true;
        clearTimeout(timeout);
        console.log('');
        console.log(`✅ Found "${TARGET_DEVICE_NAME}"!`);
        console.log(`   Address: ${address}`);
        resolve(device);
      }
    });

    // Handle discovery completion
    btSerialPort.on('finished', function() {
      if (!deviceFound) {
        clearTimeout(timeout);
        console.log('\n⚠️  Discovery finished');
        console.log(`❌ Device "${TARGET_DEVICE_NAME}" not found`);

        if (discoveredDevices.length > 0) {
          console.log('\n📱 Discovered devices:');
          discoveredDevices.forEach((dev, idx) => {
            console.log(`   ${idx + 1}. ${dev.name || 'Unknown'} (${dev.address})`);
          });
        }

        reject(new Error('Device not found'));
      }
    });

    // Start inquiry
    btSerialPort.inquire();
  });
}

/**
 * List available Bluetooth channels for a device
 */
async function listChannels(address) {
  console.log('\n🔌 Checking available SPP channels...');

  const btSerialPort = new btSerial.BluetoothSerialPort();

  return new Promise((resolve, reject) => {
    btSerialPort.findSerialPortChannel(address, function(channel) {
      if (channel > 0) {
        console.log(`   ✅ Found SPP channel: ${channel}`);
        resolve(channel);
      } else {
        console.log('   ❌ No SPP channel found');
        reject(new Error('No SPP channel available'));
      }
    }, function() {
      console.log('   ❌ Failed to find SPP channel');
      reject(new Error('Failed to query SPP channels'));
    });
  });
}

/**
 * Connect to RobotSpider Bluetooth device
 */
async function connectToRobotSpider(address, channel) {
  console.log('\n🔗 Connecting to RobotSpider...');
  console.log(`   Address: ${address}`);
  console.log(`   Channel: ${channel}`);

  const btSerialPort = new btSerial.BluetoothSerialPort();

  return new Promise((resolve, reject) => {
    btSerialPort.connect(address, channel, function() {
      console.log('   ✅ Connected successfully!');
      console.log('');
      resolve(btSerialPort);
    }, function(err) {
      console.log('   ❌ Connection failed:', err);
      reject(err);
    });
  });
}

/**
 * Main test execution
 */
async function main() {
  console.log('');
  console.log('═══════════════════════════════════════════════');
  console.log('  RobotSpider Bluetooth Integration Test');
  console.log('═══════════════════════════════════════════════');
  console.log('');

  try {
    // Step 1: Discover RobotSpider device
    const device = await discoverRobotSpider();

    // Step 2: Find SPP channel
    const channel = await listChannels(device.address);

    // Step 3: Connect to device
    const connection = await connectToRobotSpider(device.address, channel);

    // Step 4: Set up data listener (for future command responses)
    connection.on('data', function(buffer) {
      const message = buffer.toString('utf-8');
      console.log('📨 Received:', message);
    });

    connection.on('closed', function() {
      console.log('🔌 Connection closed');
    });

    connection.on('failure', function(err) {
      console.log('❌ Connection failure:', err);
    });

    console.log('═══════════════════════════════════════════════');
    console.log('  ✅ Test Completed Successfully');
    console.log('═══════════════════════════════════════════════');
    console.log('');
    console.log('💡 Connection established and ready for commands');
    console.log('   (Command sending will be implemented next)');
    console.log('');

    // Keep connection open for a few seconds to demonstrate
    console.log('⏱️  Keeping connection open for 5 seconds...');
    setTimeout(() => {
      connection.close();
      console.log('👋 Disconnected');
      process.exit(0);
    }, 5000);

  } catch (error) {
    console.log('');
    console.log('═══════════════════════════════════════════════');
    console.log('  ❌ Test Failed');
    console.log('═══════════════════════════════════════════════');
    console.log('');
    console.log('Error:', error.message);
    console.log('');
    process.exit(1);
  }
}

// Run main function
main();
