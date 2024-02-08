import 'dart:async';
import 'dart:convert';
import 'dart:math';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_ble_peripheral/flutter_ble_peripheral.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:get/get.dart';
import 'package:readair/BLE/device_details.dart';

class BluetoothPage extends StatefulWidget {
  // const BluetoothPage({super.key, required this.title});

  // final String title;

  @override
  State<BluetoothPage> createState() => _BluetoothPageState();
}

class _BluetoothPageState extends State<BluetoothPage> {
  BluetoothAdapterState blueState = BluetoothAdapterState.unknown;
  late StreamSubscription<BluetoothAdapterState> blue;

  @override
  void initState() {
    super.initState();
    blue = FlutterBluePlus.adapterState.listen((state) {
      blueState = state;
      setState(() {});
    });
    // Check if a device is already connected
    final BluetoothController bluetoothController =
        Get.find<BluetoothController>();
    bluetoothController.checkConnectedDevice(context);
  }

  @override
  void dispose() {
    blue.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          leading: IconButton(
            icon: Icon(Icons.arrow_back),
            onPressed: () => Navigator.of(context).pop(),
          ),
          title: const Text("Bluetooth"),
        ),
        body: blueState == BluetoothAdapterState.on
            ? const ScanningPage()
            : const BluetoothOffPage());
  }
}

class BluetoothOffPage extends StatelessWidget {
  const BluetoothOffPage({super.key});

  @override
  Widget build(BuildContext context) {
    return const Scaffold(
      body: Center(
        child: Text("Bluetooth is off..."),
      ),
    );
  }
}

class ScanningPage extends StatelessWidget {
  const ScanningPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: GetBuilder<BluetoothController>(
          init: BluetoothController(),
          builder: (controller) {
            return Column(
              children: [
                Center(
                  child: ElevatedButton(
                    onPressed: controller.scanDevices,
                    child: const Text("Scan"),
                  ),
                ),
                const SizedBox(height: 20),
                Expanded(
                  // Wrap StreamBuilder in an Expanded widget
                  child: StreamBuilder(
                    stream: controller.scanRes,
                    builder:
                        (context, AsyncSnapshot<List<ScanResult>> snapshot) {
                      if (snapshot.hasData) {
                        var devicesWithName = snapshot.data!
                            .where((result) =>
                                    result.device.platformName.isNotEmpty //&&
                                //result.device.platformName == "NimBLE Test")
                                )
                            .toList();

                        if (devicesWithName.isEmpty) {
                          return const Center(
                              child: Text("No named devices found!"));
                        }

                        return ListView.builder(
                          itemCount: devicesWithName.length,
                          itemBuilder: (context, i) {
                            return Card(
                              elevation: 2,
                              child: ListTile(
                                title: Text(devicesWithName[i].device.name ??
                                    'Unknown Device'),
                                subtitle: Text(
                                    devicesWithName[i].device.id.toString()),
                                trailing:
                                    Text(devicesWithName[i].rssi.toString()),
                                onTap: () => controller.connectToDevice(
                                    devicesWithName[i].device, context),
                              ),
                            );
                          },
                        );
                      } else {
                        return const Center(child: Text("No devices found!"));
                      }
                    },
                  ),
                ),
              ],
            );
          },
        ),
      ),
    );
  }
}

class BluetoothController extends GetxController {
  BluetoothDevice? connectedDevice;
  List<BluetoothService>? bluetoothServices;

  Future<void> connectToDevice(
      BluetoothDevice device, BuildContext context) async {
    try {
      await device.connect();
      connectedDevice = device;
      await requestMtuSize(device, 512);
      discoverServices();

      // Navigate to the Device Details Page
      Get.to(() => DeviceDetailsPage(
            device: device,
            onDisconnect: () => disconnectFromDevice(context),
          ));
    } catch (e) {
      // Handle connection error
      print('Error connecting to device: $e');
    }
  }

  void disconnectFromDevice(BuildContext context) async {
    if (connectedDevice != null) {
      await connectedDevice!.disconnect();
      Get.back(); // Navigate back to the previous screen
    }
  }

  Future<void> discoverServices() async {
    if (connectedDevice != null) {
      bluetoothServices = await connectedDevice!.discoverServices();
      update(); // This triggers a UI update if using GetX
      // Do something with the discovered services, like updating the UI
    }
  }

  void checkConnected(BuildContext context) {
    if (connectedDevice != null) {
      // Navigate to the Device Details Page
      Get.to(() => DeviceDetailsPage(
            device: connectedDevice!,
            onDisconnect: () => disconnectFromDevice(context),
          ));
    }
  }

  Future<void> checkConnectedDevice(BuildContext context) async {
    var connectedDevices = await FlutterBluePlus.connectedDevices;
    var filteredDevices =
        connectedDevices.where((d) => d.name == 'NimBLE Test');

    if (connectedDevices.isNotEmpty) {
      BluetoothDevice targetDevice = connectedDevices.first;
      connectedDevice = targetDevice;
      await discoverServices();
      Get.off(() => DeviceDetailsPage(
            device: targetDevice,
            onDisconnect: () => disconnectFromDevice(context),
          ));
    }
  }

    Future<void> subscribeToDevice(BluetoothDevice device) async {
    try {
      var services = await device.discoverServices();
      for (var service in services) {
        var characteristics = service.characteristics;
        for (var characteristic in characteristics) {
          // Assuming these UUIDs are the ones you're interested in
          if (characteristic.uuid == Guid("588d30b0-33aa-4654-ab36-56dfa9974b13")) {
            // Setup notifications
            await characteristic.setNotifyValue(true);
            characteristic.value.listen((value) {
              // Handle incoming data
              String receivedData = String.fromCharCodes(value);
              print("Received data: $receivedData");
              // You might want to update the UI or process data here
            });
            print("Subscribed to characteristic");
            update(); // Update UI if necessary
          }
        }
      }
    } catch (e) {
      print("Error subscribing to device: $e");
    }
  }

  Future<void> requestMtuSize(BluetoothDevice device, int requestedMtu) async {
    try {
      int resultingMtu = await device.requestMtu(requestedMtu);
      print('MTU size after negotiation: $resultingMtu');
    } catch (e) {
      print('Error requesting MTU size: $e');
    }
  }

  Future scanDevices() async {
    FlutterBluePlus.startScan(timeout: const Duration(seconds: 5));
    // FlutterBluePlus.stopScan();
  }

  Stream<List<ScanResult>> get scanRes => FlutterBluePlus.scanResults;
}

// class BlePeripheralProvider with ChangeNotifier {
//   final FlutterBlePeripheral _blePeripheral = FlutterBlePeripheral();
//   BluetoothCharacteristic? _rwCharacteristic;
//   late AdvertiseData _advertiseData;
//   late AdvertiseSettings _advertiseSettings;

//   Uint8List? _otaData;
//   int _otaOffset = 0;

//   bool _isAdvertising = false; // Default value

//   bool get isAdvertising => _isAdvertising;

//   final String _serviceUuid = "2e93ce4d-58db-42d0-ae83-8f8ae1d74e0d";
//   final String _characteristicUuid = "f6b7a0ba-547b-4607-bae2-0d6ec26ff5cf";

//   BlePeripheralProvider() {
//     _advertiseData = AdvertiseData(
//       serviceUuid: _serviceUuid, // The UUID of the service
//       includeDeviceName: true,
//       includePowerLevel: true,
//       manufacturerData: Uint8List.fromList(utf8.encode('FlutterBlePeripheral')),
//     );

//     void setRwCharacteristic(BluetoothCharacteristic characteristic) {
//       _rwCharacteristic = characteristic;
//     }

//     // Initialize the advertising settings
//     _advertiseSettings = AdvertiseSettings(
//       timeout: 10000,
//     );

//     _blePeripheral.onPeripheralStateChanged?.listen((event) {
//       // Update isAdvertising based on the event
//       if (event == PeripheralState.advertising) {
//         _isAdvertising = true;
//         print("Advertising data");
//       } else if (event == PeripheralState.idle) {
//         _isAdvertising = false;
//         print("Not advertising");
//       }
//       notifyListeners(); // Notify listeners about the change
//     });
//     // Subscribe to peripheral state changes
//     _blePeripheral.onPeripheralStateChanged?.listen((event) {
//       if (event == PeripheralState.advertising) {
//         print("Advertising data");
//       } else if (event == PeripheralState.idle) {
//         print("Not advertising");
//       }
//       notifyListeners();
//     });
//   }

// Future<void> startAdvertising() async {
//   try {
//     print("Attempting to start advertising...");
//     await _blePeripheral.start(
//       advertiseData: _advertiseData,
//       advertiseSettings: _advertiseSettings,
//     );
//     print("Started Advertising");
//   } catch (e) {
//     print("Failed to start advertising: $e");
//   }
//   notifyListeners();
// }


//   Future<void> stopAdvertising() async {
//     try {
//       await _blePeripheral.stop();
//       print("Stopped Advertising");
//       _isAdvertising = false; // Update the flag
//     } catch (e) {
//       print("Failed to stop advertising: $e");
//     }
//     notifyListeners();
//   }

//   Future<bool> checkBinFileAvailability() async {
//     try {
//       final byteData = await rootBundle.load('assets/HomeAir.ino.bin');
//       print("File is available");
//       return true;
//     } catch (e) {
//       print("Error: File not available - $e");
//       return false;
//     }
//   }

//   Future<void> startOtaUpdate(Uint8List otaData) async {
//     try {
//       await _sendData('SERV!');
//       print("SERV! sent");

//       _otaData = otaData;
//       _otaOffset = 0; // Reset offset
//       notifyListeners(); // Notify UI or other components
//       print("OTA update prepared. Waiting for read requests...");
//     } catch (e) {
//       print("Error during firmware update: $e");
//     }
//   }

//   // Future<void> startOtaUpdate(Uint8List otaData) async {
//   //   try {
//   //     // Notify the client (ESP) that the server will start sending the OTA update
//   //     await _sendData('SERV!');
//   //     print("SERV! sent");

//   //     int fileSize = otaData.length;
//   //     int chunkSize = 500; // Define the size of each chunk
//   //     int bytesTransferred = 0;

//   //     // Split the binary data into chunks and send each chunk
//   //     for (int i = 0; i * chunkSize < fileSize; i++) {
//   //       int start = i * chunkSize;
//   //       int end = min(fileSize, (i + 1) * chunkSize);
//   //       Uint8List chunk = otaData.sublist(start, end);

//   //       // Send the chunk to the ESP (client)
//   //       await _sendChunk(chunk);
//   //       bytesTransferred += chunk.length;

//   //       double progress = (bytesTransferred / fileSize) * 100;
//   //       if (progress >= 5 && progress % 5 < 0.1) {
//   //         print("${progress.toInt()}% uploaded.");
//   //       }

//   //       // Add a delay if necessary to ensure the ESP has time to process the chunk
//   //       await Future.delayed(Duration(milliseconds: 20));
//   //     }

//   //     // Notify the completion of the file transfer
//   //     print("File upload complete.");
//   //     await _sendData("DONE!");
//   //   } catch (e) {
//   //     print("Error during firmware update: $e");
//   //   }
//   // }

//   Future<void> _sendChunk(Uint8List chunk) async {
//     if (_rwCharacteristic != null) {
//       await _rwCharacteristic!.write(chunk, withoutResponse: false);
//     } else {
//       print("Write characteristic is not set.");
//     }
//   }

//   void setOtaData(Uint8List otaData) {
//     _otaData = otaData;
//     _otaOffset = 0; // Reset offset
//     notifyListeners();
//   }

//   Uint8List onReadRequest(int offset) {
//     if (_otaData != null && _otaOffset < _otaData!.length) {
//       int end = min(_otaOffset + 500, _otaData!.length); // Define chunk size
//       Uint8List chunk = _otaData!.sublist(_otaOffset, end);
//       _otaOffset = end; // Update offset
//       print("Serving OTA chunk. Offset: $_otaOffset");
//       return chunk;
//     } else {
//       print("All OTA data has been served.");
//       return Uint8List.fromList([]); // Return an empty array when done
//     }
//   }

//   Future<void> _sendData(String data) async {
//     if (_rwCharacteristic != null) {
//       List<int> bytesToSend = utf8.encode(data);
//       await _rwCharacteristic!.write(bytesToSend, withoutResponse: false);
//       print("Sent data: $data");
//     } else {
//       print("Characteristic is not set.");
//     }
//   }

//   @override
//   void dispose() {
//     _blePeripheral.stop();
//     super.dispose();
//   }
// }
