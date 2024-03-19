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
import 'package:readair/data/packet.dart';
import 'package:readair/main.dart';

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
  RxBool isSubscribed = false.obs;
  Timer? _updatTimer;

  void showGlobalSnackBar(String message) {
    scaffoldMessengerKey.currentState?.showSnackBar(
      SnackBar(content: Text(message)),
    );
  }

  Future<void> connectToDevice(
      BluetoothDevice device, BuildContext context) async {
    try {
      await device.connect();
      connectedDevice = device;
      await requestMtuSize(device, 512);
      discoverServices();
      await subscribeToDevice(device);

      @override
      void onInit() {
        super.onInit();
        // Initialize the timer or other resources if needed
      }

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
      isSubscribed.value = false; // Reset subscription status
      _updatTimer?.cancel(); // Stop sending "UPDAT"
      update(); // Update UI
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

  void _processDataPacket(String data) {
    // Split the incoming data by new lines to handle multiple packets
    var packets = data.trim().split('\n');
    for (var packetData in packets) {
      try {
        var parsedData = packetData.split(',');
        // Adjust the condition to match the correct length for your packets
        if ((parsedData.length == 12) || (parsedData.length == 13)) {
          var packet = DataPacket(
            epochTime: double.tryParse(parsedData[0]) ?? 0.0,
            co2: double.tryParse(parsedData[1]) ?? 0.0,
            ppm1_0: double.tryParse(parsedData[2]) ?? 0.0,
            ppm2_5: double.tryParse(parsedData[3]) ?? 0.0,
            ppm4_0: double.tryParse(parsedData[4]) ?? 0.0,
            ppm10_0: double.tryParse(parsedData[5]) ?? 0.0,
            humid: double.tryParse(parsedData[6]) ?? 0.0,
            temp: double.tryParse(parsedData[7]) ?? 0.0,
            voc: double.tryParse(parsedData[8]) ?? 0.0,
            nox: double.tryParse(parsedData[9]) ?? 0.0,
            co: double.tryParse(parsedData[10]) ?? 0.0,
            ng: double.tryParse(parsedData[11]) ?? 0.0,
            aqi: double.tryParse(parsedData[12]) ?? 0.0,
          );

          print("Received Packet: $packetData");
          DatabaseService.instance.insertOrUpdateDataPacket(packet);
        } else {
          // If the received data does not match expected format, show a global SnackBar
          //showGlobalSnackBar("Received data does not match expected format.");
        }
      } catch (e) {
        print("Error processing packet: $e");
        //showGlobalSnackBar("Error processing packet: $e");
      }
    }
  }

  void _showMessage(String message) {
    scaffoldMessengerKey.currentState
        ?.showSnackBar(SnackBar(content: Text(message)));
  }

  Future<void> subscribeToDevice(BluetoothDevice device) async {
    try {
      // Ensure the device is connected before attempting to communicate
      if (connectedDevice == null) {
        print("Device is not connected.");
        return;
      }

      // Send current time to ESP32
      await _sendTimeToEsp32(device);
      await Future.delayed(Duration(milliseconds: 250));

      // Send 'TGMT=-7' command
      await sendData(device, 'TGMT=-7');
      await Future.delayed(Duration(milliseconds: 250));

      // Proceed with discovering services and subscribing to the characteristic
      var services = await device.discoverServices();
      for (var service in services) {
        for (var characteristic in service.characteristics) {
          if (characteristic.properties.notify) {
            await characteristic.setNotifyValue(true);
            characteristic.value.listen((value) {
              String receivedData = String.fromCharCodes(value);
              _processDataPacket(receivedData);
            });
            isSubscribed.value = true;
            update(); // Notify listeners about the change
            print("Subscribed to characteristic: ${characteristic.uuid}");
            break; // Exit the loop once subscribed
          }
        }
      }

      sendData(device, "READ!");

      _updatTimer?.cancel(); // Cancel any existing timer
      _updatTimer = Timer.periodic(Duration(seconds: 10), (timer) async {
        await sendData(device, "UPDAT");
        print("Sent UPDAT");
      });
    } catch (e) {
      print("Error in subscription process: $e");
      isSubscribed.value = false;
      update(); // Notify listeners about the change
    }
  }

  // Helper method to send data to the ESP32
  Future<void> sendData(BluetoothDevice device, String data) async {
    var services = await device.discoverServices();
    for (var service in services) {
      var characteristic = service.characteristics.firstWhereOrNull(
          (c) => c.uuid == Guid("588d30b0-33aa-4654-ab36-56dfa9974b13"));
      if (characteristic != null) {
        await characteristic.write(utf8.encode(data));
        print("Sent $data to device");
        return;
      }
    }
    print("Characteristic for sending data not found.");
  }

  // Assumes _sendTimeToEsp32 is similar to _sendData but specifically for sending the current time
  Future<void> _sendTimeToEsp32(BluetoothDevice device) async {
    int epochTime = DateTime.now().millisecondsSinceEpoch ~/ 1000;
    await sendData(device, "TIME=$epochTime");
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

  @override
  void onClose() {
    // Clean up resources
    _updatTimer?.cancel();
    super.onClose();
  }

  Stream<List<ScanResult>> get scanRes => FlutterBluePlus.scanResults;
}
