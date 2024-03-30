import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'dart:math';
import 'package:file_picker/file_picker.dart';
import 'package:readair/data/packet.dart';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:collection/collection.dart';
import 'package:flutter/services.dart' show Uint8List, rootBundle;

class DeviceDetailsPage extends StatefulWidget {
  final BluetoothDevice device;
  final VoidCallback onDisconnect;

  const DeviceDetailsPage({
    Key? key,
    required this.device,
    required this.onDisconnect,
  }) : super(key: key);

  @override
  State<DeviceDetailsPage> createState() => _DeviceDetailsPageState();
}

class _DeviceDetailsPageState extends State<DeviceDetailsPage> {
  List<BluetoothService> _services = [];
  Timer? _timer;
  String? _latestReceivedPacket;
  bool isSubscribed = false;
  int _connectionStep = 0;
  BluetoothCharacteristic? writeCharacteristic;
  @override
  void initState() {
    super.initState();
    _discoverServices();
    _subscribeToDevice();

    //_setupPeriodicReadCommand();
    _latestReceivedPacket = '';
  }

  @override
  void dispose() {
    _timer?.cancel();
    isSubscribed = false;
    super.dispose();
  }

  void _setupPeriodicReadCommand() {
    //_timer = Timer.periodic(Duration(minutes: 1), (Timer t) => _sendReadCommand());
  }

  Future<void> _sendReadCommand() async {
    await _sendData("READ!");
  }

  Future<void> _initializeEsp32Connection() async {
    // Send current time to ESP32
    await _sendTimeToEsp32();
    await Future.delayed(Duration(seconds: 1));

    // Send 'TGMT=-7' command
    await _sendData('TGMT=-7');
    await Future.delayed(Duration(seconds: 1));

    // Subscribe to ESP32
    await _subscribeToDevice();
  }

  Future<void> _discoverServices() async {
    List<BluetoothService> services = await widget.device.discoverServices();
    for (BluetoothService service in services) {
      var characteristics = service.characteristics;
      for (BluetoothCharacteristic characteristic in characteristics) {
        if (characteristic.uuid ==
            Guid("588d30b0-33aa-4654-ab36-56dfa9974b13")) {
          writeCharacteristic = characteristic;
          print("Write characteristic found");
          // Once found, you can break out of the loop
          return;
        }
      }
    }
  }

  Future<void> _setupNotification() async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    final targetService =
        _services.firstWhereOrNull((s) => s.uuid == serviceUuid);

    if (targetService != null) {
      var characteristic = targetService.characteristics
          .firstWhereOrNull((c) => c.uuid == characteristicUuid);

      if (characteristic != null) {
        await characteristic.setNotifyValue(true);
        characteristic.value.listen((value) {
          // Process incoming data packet
          String receivedData = String.fromCharCodes(value);
          _latestReceivedPacket = receivedData;
          _processDataPacket(receivedData);

          // Show a notification message
          _showNotification("Data received from ESP32");
        });
        setState(() {
          isSubscribed = true;
        });
      }
    }
  }

  void _showNotification(String message) {
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(
      content: Text(message),
      duration: Duration(seconds: 2),
    ));
  }

  void _processDataPacket(String data) {
    // Split the incoming data by new lines to handle multiple packets
    var packets = data.trim().split('\n');
    for (var packetData in packets) {
      try {
        var parsedData = packetData.split(',');
        if (parsedData.length == 12) {
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
            co: double.tryParse(parsedData[9]) ?? 0.0,
            ng: double.tryParse(parsedData[10]) ?? 0.0,
            aqi: double.tryParse(parsedData[11]) ?? 0.0,
          );

          print(packetData);
          DatabaseService.instance.insertOrUpdateDataPacket(packet);
        } else {
          _showMessage("Received data does not match expected format.");
        }
      } catch (e) {
        _showMessage("Error processing packet: $e");
      }
    }
  }

  void _showMessage(String message) {
    ScaffoldMessenger.of(context)
        .showSnackBar(SnackBar(content: Text(message)));
  }

  void _showReceivedDataDialog(String data) {
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: Text('Received Data'),
          content: Text(data),
          actions: <Widget>[
            TextButton(
              child: Text('Close'),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
  }

  Future<void> _customEsp32ConnectionSequence() async {
    await _initializeEsp32Connection();
    await Future.delayed(Duration(seconds: 3));

    await _sendReadCommand();
    await Future.delayed(Duration(seconds: 7));

    await _receiveAndReadData();

    _timer = Timer.periodic(Duration(seconds: 10), (Timer t) async {
      await _receiveAndReadData();
    });
  }

  Future<void> _autoReadOnNotification() async {
    await _initializeEsp32Connection();
    await Future.delayed(Duration(seconds: 3));
    await _sendReadCommand();
    await Future.delayed(Duration(seconds: 7));

    _setupNotificationWithAutoRead();
  }

  void _setupNotificationWithAutoRead() async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    final targetService =
        _services.firstWhereOrNull((s) => s.uuid == serviceUuid);

    if (targetService != null) {
      var characteristic = targetService.characteristics
          .firstWhereOrNull((c) => c.uuid == characteristicUuid);

      if (characteristic != null) {
        await characteristic.setNotifyValue(true);
        characteristic.value.listen((value) {
          String receivedData = String.fromCharCodes(value);
          _latestReceivedPacket = receivedData;
          _processDataPacket(receivedData);

          _showNotification("Data received from ESP32");

          _receiveAndReadData();
        });
        setState(() {
          isSubscribed = true;
        });
      }
    }
  }

  void _setupNotificationForImmediateRead() async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    final targetService =
        _services.firstWhereOrNull((s) => s.uuid == serviceUuid);

    if (targetService != null) {
      var characteristic = targetService.characteristics
          .firstWhereOrNull((c) => c.uuid == characteristicUuid);

      if (characteristic != null) {
        await characteristic.setNotifyValue(true);
        characteristic.value.listen((value) {
          // As soon as data is received, process and read the next packet
          String receivedData = String.fromCharCodes(value);
          _latestReceivedPacket = receivedData;
          _processDataPacket(receivedData);

          // Immediately trigger the read data function
          _receiveAndReadData();
        });
        setState(() {
          isSubscribed = true;
        });
      }
    }
  }

  Future<void> _autoReadOnDataReceive() async {
    // Initialize connection
    await _initializeEsp32Connection();
    await Future.delayed(Duration(seconds: 3));
    await _sendReadCommand();
    await Future.delayed(Duration(seconds: 7));

    _setupNotificationForImmediateRead();
  }

  Future<void> _autoReads() async {
    await _sendReadCommand();
    //await Future.delayed(Duration(seconds: 5));

    _setupNotificationForImmediateRead();
  }

  Future<void> _sendData(String dataToSend) async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    // Convert the String to a list of bytes
    List<int> bytesToSend = utf8.encode(dataToSend);

    final targetService = _services.firstWhereOrNull(
      (s) => s.uuid == serviceUuid,
    );

    if (targetService != null) {
      var characteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == characteristicUuid,
      );

      if (characteristic != null) {
        await characteristic.write(bytesToSend, withoutResponse: false);
      }
    }
  }

  Future<void> _receiveAndReadData() async {
    final writeCharacteristicUuid =
        Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");
    final readCharacteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");

    final targetService = _services.firstWhereOrNull(
      (s) => s.uuid == serviceUuid,
    );

    if (targetService != null) {
      var writeCharacteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == writeCharacteristicUuid,
      );

      var readCharacteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == readCharacteristicUuid,
      );

      if (writeCharacteristic != null && readCharacteristic != null) {
        // Read the response
        await readCharacteristic.read().then((value) {
          String receivedData = String.fromCharCodes(value);
          //_showMessage(receivedData);
          _latestReceivedPacket = receivedData;
          _processDataPacket(receivedData);
        });
      }
    }
  }

  Future<void> _sendAndReadData() async {
    final writeCharacteristicUuid =
        Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");
    final readCharacteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");

    // Convert the String "READ!" to a list of bytes
    List<int> bytesToSend = utf8.encode("READ!");

    final targetService = _services.firstWhereOrNull(
      (s) => s.uuid == serviceUuid,
    );

    if (targetService != null) {
      var writeCharacteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == writeCharacteristicUuid,
      );

      var readCharacteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == readCharacteristicUuid,
      );

      if (writeCharacteristic != null && readCharacteristic != null) {
        // Write "READ!" to the ESP32
        await writeCharacteristic.write(bytesToSend, withoutResponse: false);
        // Read the response
        await readCharacteristic.read().then((value) {
          String receivedData = String.fromCharCodes(value);
          _showReceivedDataDialog(receivedData);
        });
      }
    }
  }

  Future<void> _subscribeToDevice() async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    final targetService =
        _services.firstWhereOrNull((s) => s.uuid == serviceUuid);

    if (targetService != null) {
      var characteristic = targetService.characteristics
          .firstWhereOrNull((c) => c.uuid == characteristicUuid);

      if (characteristic != null) {
        await characteristic.setNotifyValue(true);
        characteristic.value.listen((value) {
          String receivedData = String.fromCharCodes(value);
          _latestReceivedPacket = receivedData;
          _processDataPacket(receivedData);
          _showNotification("Data received from ESP32");
        });

        setState(() {
          isSubscribed = true;
        });
      }
    }
  }

  Future<void> _sendTimeToEsp32() async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    // Get the current epoch time in seconds.
    int epochTime = DateTime.now().millisecondsSinceEpoch ~/ 1000;

    // Create the command string.
    String timeCommand = "TIME=$epochTime";

    // Convert the command string to bytes.
    List<int> bytesToSend = utf8.encode(timeCommand);

    // Find the service.
    final targetService = _services.firstWhereOrNull(
      (s) => s.uuid == serviceUuid,
    );

    if (targetService != null) {
      // Find the characteristic.
      var characteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == characteristicUuid,
      );

      if (characteristic != null) {
        // Write the time command to the ESP32.
        await characteristic.write(bytesToSend, withoutResponse: false);
      }
    }
  }

  Future<List<int>> loadBinFile() async {
    final byteData = await rootBundle.load('assets/HomeAir.ino.bin');
    return byteData.buffer.asUint8List();
  }

  Future<void> sendAssetFile() async {
    try {
      List<int> fileBytes = await loadBinFile();
      Uint8List uint8list = Uint8List.fromList(fileBytes);
      File tempFile = File.fromRawPath(uint8list);
      await sendFileInChunks(tempFile);
    } catch (e) {
      _showMessage("Error loading asset file: $e");
    }
  }

  Future<void> pickAndSendFile() async {
    FilePickerResult? result = await FilePicker.platform
        .pickFiles(type: FileType.custom, allowedExtensions: ['bin']);

    if (result != null) {
      PlatformFile file = result.files.first;

      String? filePath = file.path;

      if (filePath == null) {
        _showMessage("No file selected.");
        return;
      }

      // Now you can use the filePath to read the file in chunks and send it
      File binFile = File(filePath);
      await sendFileInChunks(binFile);
    } else {
      // User canceled the picker
      _showMessage("File pick cancelled.");
    }
  }

  Future<void> sendFileInChunks(File file) async {
    final fileSize = await file.length();
    final fileBytes = await file.readAsBytes();
    final blockSize = 512; // BLE packet size
    int bytesTransferred = 0;

    for (int i = 0; i * blockSize < fileSize; i++) {
      int start = i * blockSize;
      int end = min(fileSize, (i + 1) * blockSize);
      List<int> chunk = fileBytes.sublist(start, end);

      await writeCharacteristic!.write(chunk, withoutResponse: true);
      bytesTransferred += chunk.length;

      // Update progress
      double progress = (bytesTransferred / fileSize) * 100;
      if (progress >= 5 && progress % 5 < 0.1) {
        // Update every 5%
        _showMessage("${progress.toInt()}% uploaded.");
      }

      // Add a delay if needed
      await Future.delayed(Duration(milliseconds: 20));
    }

    _showMessage("File upload complete.");
  }

  Future<void> _readLatestPacket() async {
    var packet = await DatabaseService.instance.getLastPacket();
    if (packet != null) {
      _showReceivedDataDialog(packet.toString());
    } else {
      _showReceivedDataDialog("No data available");
    }
  }

  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Device Details'),
      ),
      body: Padding(
        padding: EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text('Device Name: ${widget.device.name ?? 'Unknown'}',
                style: TextStyle(fontSize: 20)),
            SizedBox(height: 8),
            Text('Device ID: ${widget.device.id.id}',
                style: TextStyle(fontSize: 20)),
            SizedBox(height: 24),
            Text('Subscribed: ${isSubscribed ? "Yes" : "No"}',
                style: TextStyle(
                    fontSize: 20,
                    color: isSubscribed ? Colors.green : Colors.red)),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: _initializeEsp32Connection,
                child: Text('Initialize ESP32 Connection'),
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: _customEsp32ConnectionSequence,
                child: Text('Timed Read initialization'),
              ),
            ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: _autoReadOnNotification,
            //     child: Text('Auto Read on Notification'),
            //   ),
            // ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //       onPressed: () {
            //         _sendData('TGMT=-7');
            //       },
            //       child: Text('Send Data')),
            // ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: _sendReadCommand,
            //     child: Text('Send "READ!"'),
            //   ),
            // ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: _autoReadOnDataReceive,
                child: Text('Read on Data Receive'),
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: _autoReads,
                child: Text('Start Reading'),
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: () {
                  _sendData("UPDAT");
                },
                child: Text('Update'),
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: sendAssetFile, // Change this
                child: Text('Send Asset File'),
              ),
            ),

            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: _receiveAndReadData,
            //     child: Text('Read Data'),
            //   ),
            // ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: _sendTimeToEsp32,
            //     child: Text('Set Time on ESP32'),
            //   ),
            // ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: _subscribeToDevice,
            //     child: Text('Subscribe to ESP32'),
            //   ),
            // ),

            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: widget.onDisconnect,
                child: Text('Disconnect'),
              ),
            ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: _addRandomPacket,
            //     child: Text('Add Random Packet'),
            //   ),
            // ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: _readLatestPacket,
            //     child: Text('Read Latest Packet'),
            //   ),
            // ),
          ],
        ),
      ),
    );
  }
}
