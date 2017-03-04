#pragma once

#include <SFML/Network.hpp>
#include <thread>
#include <mutex>
#include <iostream>

#include "ship.hpp"
#include "bullet.hpp"
#include "shipHull.hpp"
#include "BulletHull.hpp"
#include "objectCollisonManager.hpp"
//#include "Tower.hpp"
//#include "TowerHull.hpp"

using namespace std;
using namespace sf;

bool isRunning, isStopping, connected;

IpAddress target;
unsigned short port;
TcpSocket socket;
TcpListener listener;

Packet packetCache;
mutex packetLock, socketLock;

bool isFrameReady = true;

class NetworkInterface
{
public:

	static string startThread(int listen, IpAddress Target, unsigned short Port, bool inputFromStdin = false)
	{
		isRunning = isStopping = connected = false;
		cout << endl << "Network thread starting..." << endl;
		if (inputFromStdin)
		{
			string ipCache;
			cout << "Listen (1), connect (0) or cancel (2)? ";
			cin >> listen;
			if (listen == 2) {
				cout << "Network thread not activated. Returning..." << endl;
				return "FAILED: Network thread not activated.";
			}
			if (listen == 0) //Connects
			{
				cout << "Target IP Address: ";
				cin >> ipCache;
				Target = ipCache;
			}
			cout << "Target Port: ";
			cin >> Port;
		}
		if (listen == 1)
		{
			port = Port;
			thread thread2(threadListener);
			thread2.detach();
			char tmp[1024];
			sprintf(tmp, "SUCCESS: Start Listening on port %d.", port);
			return string(tmp);
		}
		else if (listen == 0)
		{
			target = Target;
			port = Port;
			return threadConnector();
		}

	}

	static void threadListenerInThread() {
		thread thread2(threadListener);
		thread2.detach();
	}

	static void threadListener()
	{
		listener.setBlocking(true);
		listener.listen(port);
		cout << "Target now listening on port " << port << "." << endl;
		listener.accept(socket);
		listener.close();
		target = socket.getRemoteAddress();
		port = socket.getRemotePort();
		//socket.setBlocking(false);
		cout << "Target hit: " << target.toString() << ":" << port << endl;
		connected = true;
		isStopping = false;
		isRunning = true;
		thread thread1(threadWorker); //std::thread once used.
		thread1.detach();
	}

	static string threadConnector()
	{
		if (target == IpAddress::Any || target == IpAddress::None)
			return "FAILED: Unknown host.";
		cout << "Connecting to Target " << target.toString() << ":" << port << endl;
		Socket::Status stat = socket.connect(target, port, seconds(5.0f));
		//socket.setBlocking(false);
		char tmp[1024];
		if (stat == Socket::Done) {
			isRunning = true;
			isStopping = false;
			connected = true;
			thread thread1(threadWorker); //std::thread once used.
			thread1.detach();
			sprintf(tmp, "SUCCESS: Target hit: %s:%d", target.toString().c_str(), port);
		}
		else
			sprintf(tmp, "FAILED: Target connection failed.");
		return string(tmp);
	}

	static Packet getPacket()
	{
		Packet pack;
		int n = 0;
		for (int i = 0; i < bulletCount; i++)
			if (bullet[i].isAlive())
				n++;
		pack.clear();
		//pack << n;
		for (int i = 0; i < bulletCount; i++)
		{
			if (bullet[i].isAlive())
				pack << bullet[i].getPosition().x << bullet[i].getPosition().y << bullet[i].size << bullet[i].getRotation();
		}
		pack << -1.0f << -1.0f;
		//pack << 1;
		pack << ship.getPosition().x << ship.getPosition().y;
		pack << ship.getRotation() << ship.getName();
		pack << ship.isDocked() << (isFire == Ship::Back) << isExplode << isDead;
		pack << -1.0f << -1.0f;
		int p = 0;
		//for (Tower& i : towerList.tower) {
		//	pack << i.getPosition().x << i.getPosition().y << i.getRotation();
		//	p++;
			//cout << "Packaged Tower. Number: " << p << endl;
		//}
		//pack << -1.0f << -1.0f;
		int partN = particleManager.partEmitTrace.size();
		pack << partN;
		while (partN--) {
			particleManager.partEmitTraceLock.lock();
			ParticleManager::ParticleEmitTrace trace = particleManager.partEmitTrace.front();
			particleManager.partEmitTrace.pop();
			particleManager.partEmitTraceLock.unlock();
			pack << trace.type;
			if (trace.type == ParticleManager::randomSpeedAndDirection) {
				pack << trace.posX << trace.posY;
				pack << trace.color.r << trace.color.g << trace.color.b;
				pack << trace.count;
			}
			else if (trace.type == ParticleManager::randomDirection) {
				pack << trace.posX << trace.posY;
				pack << trace.color.r << trace.color.g << trace.color.b;
				pack << trace.count;
				pack << trace.speed;
			}
			else if (trace.type == ParticleManager::setSpeedAndDirection) {
				pack << trace.posX << trace.posY;
				pack << trace.color.r << trace.color.g << trace.color.b;
				pack << trace.count;
				pack << trace.speed;
				pack << trace.angle << trace.angleFlow;
			}
		}
		return pack;  //int float float int int ... int [float float int sf::String bool bool bool bool]
					  //int(n) repeatN[ int(type) double double ... ]
	}

	static void processPacketIntoData(Packet pack) {
		//Cout << "Decoding Packet" << endl;
		Packet copy = pack;
		if (!connected)
			return;
		float offX, offY, sizeX, sizeY;
		int size, rot;
		String name;
		bool isDocked, isFire, isExplode, isDead;
		int curPos = 0;
		int n;
		//copy >> n;
		while (true)
		{
			copy >> offX >> offY;
			if (isSame(offX, -1.0f) && isSame(offY, -1.0f))
				break;
			copy >> size >> rot;
			//Bullet::drawBullet(win, offX, offY, size, rot);
			bulletHull[curPos] = BulletHull(offX, offY, rot, size);
			curPos++;
		}
		bulletHullAliveCount = curPos;
		curPos = 0;
		while (true) {
			copy >> offX >> offY;
			if (isSame(offX, -1.0f) && isSame(offY, -1.0f))
				break;
			copy >> rot >> name;
			copy >> isDocked >> isFire >> isExplode >> isDead;
			shipHull[curPos] = ShipHull(offX, offY, rot, name.toAnsiString(locale()), isDocked, isFire, isDead);
			curPos++;
		}
		shipHullAliveCount = curPos;
		int p = 0;
		//towerHulls.clear();
		//while (true) {
		//	p++;
		//	copy >> offX >> offY;
		//	if (isSame(offX, -1.0f) && isSame(offY, -1.0f))
		//		break;
		//	copy >> rot;
		//	towerHulls.push_back(TowerHull(Vector2d(offX, offY), rot));
		//}
		int partN = particleManager.partEmitTrace.size();
		copy >> partN;
		while (partN--) {
			//Cout << "Decoding Particle Traces... " << partN + 1 << " left." << endl;
			int t;
			ParticleManager::ParticleEmitTrace trace;
			copy >> trace.type;
			if (trace.type == ParticleManager::randomSpeedAndDirection) {
				copy >> trace.posX >> trace.posY;
				copy >> trace.color.r >> trace.color.g >> trace.color.b;
				copy >> trace.count;
				//Cout << "Particle Trace Type: randomSpeedAndDirection" << endl;
			}
			else if (trace.type == ParticleManager::randomDirection) {
				copy >> trace.posX >> trace.posY;
				copy >> trace.color.r >> trace.color.g >> trace.color.b;
				copy >> trace.count;
				copy >> trace.speed;
				//Cout << "Particle Trace Type: randomDirection" << endl;
			}
			else if (trace.type == ParticleManager::setSpeedAndDirection) {
				copy >> trace.posX >> trace.posY;
				copy >> trace.color.r >> trace.color.g >> trace.color.b;
				copy >> trace.count;
				copy >> trace.speed;
				copy >> trace.angle >> trace.angleFlow;
				//Cout << "Particle Trace Type: setSpeedAndDirection" << endl;
			}
			//Cout << "Trying to emit particles." << endl;
			particleManager.emit(trace);
			//Cout << "Particle emited; wait until next render." << endl;
		}
		//Ship::drawShip(win, offX, offY, rot, name.toAnsiString(locale("GBK")), isDocked, isFire);
	}

	/*static void renderFromPacket(RenderWindow& win, Packet pack)
	{
		float offX, offY, sizeX, sizeY;
		int size, rot;
		String name;
		bool isDocked, isFire, isExplode, isDead;
		while (true)
		{
			pack >> offX >> offY;
			if (!pack)
				return;
			if (isSame(offX, -1.0f) && isSame(offY, -1.0f))
				break;
			pack >> size >> rot;
			Bullet::drawBullet(win, offX, offY, size, rot);
		}
		pack >> offX >> offY >> rot >> name;
		pack >> isDocked >> isFire >> isExplode >> isDead;
		if (!isDead)
			Ship::drawShip(win, offX, offY, rot, name.toAnsiString(locale("GBK")), isDocked, isFire);
	}*/

	static void threadSender()
	{
		if (!connected)
			return;
		logicDataLock.lock();
		Packet pack = getPacket();
		logicDataLock.unlock();
		Socket::Status status;
		//Cout << "Sending packet." << endl;
		//do
		//{
		status = socket.send(pack);
		//} while (status != Socket::Partial);
		if (status == Socket::Disconnected)
			connected = false;
		//Cout << "Sent packet." << endl;
	}

	static void threadReceiver()
	{
		if (!connected)
			return;
		Packet packetCopy;
		Socket::Status status;
		//Cout << "Receiving packet." << endl;

		status = socket.receive(packetCopy);
		if (status == Socket::Disconnected)
			connected = false;

		//Cout << "Received packet." << endl;
		packetLock.lock();
		packetCache = packetCopy;
		packetLock.unlock();

		logicDataLock.lock();
		processPacketIntoData(packetCopy);
		logicDataLock.unlock();
	}

	static void threadWorker()
	{
		if (!connected)
			return;
		//Cout << "threadWorker() starting." << endl;
		isRunning = true;
		isStopping = false;
		connected = true;
		//socket.setBlocking(false);
		while (isStopping == false)
		{
			logicDataLock.lock();
			if (!isFrameReady) {
				logicDataLock.unlock();
				sleep(milliseconds(1));
				continue;
			}
			else
				logicDataLock.unlock();
			threadSender();
			threadReceiver();
			if (!connected)
				break;
			objectCollisonManager.updateLogic(win);
			isFrameReady = false;
		}
		socket.disconnect();
		isRunning = false;
		isStopping = false;
		//Cout << "threadWorker() stopping." << endl;

	}

	static void threadWorkerNoThread() {
		//Cout << "threadWorkerNoThread() invoked." << endl;
		if (!connected || !isRunning)
			return;
		if (isStopping) {
			socket.disconnect();
			isRunning = false;
			isStopping = false;
			//Cout << "threadWorker() stopping." << endl;
		}
		//socket.setBlocking(false);
		threadSender();
		threadReceiver();
		objectCollisonManager.updateLogic(win);
	}

	static void threadRenderer(RenderWindow& win)
	{
		if (!connected)
			return;
		//for (TowerHull& i : towerHulls)
		//	i.onRender(win);
		for (int i = 0; i < bulletHullAliveCount; i++)
			bulletHull[i].onRender(win);
		for (int i = 0; i < shipHullAliveCount; i++)
			shipHull[i].onRender(win);
	}

	static void stopThread()
	{
		if (isRunning)
			isStopping = true;
	}

	static void stopNoThread()
	{
		socket.disconnect();
		isRunning = false;
		isStopping = false;
		//Cout << "threadWorkerNoThread() stopping." << endl;
	}

	static const bool running()
	{
		return isRunning;
	}


};
