from setuptools import find_packages, setup
from glob import glob
import os

package_name = 'ad4114exg_publisher'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Ioan Dragomir',
    maintainer_email='ioan.dragomir@analog.com',
    description='ROS2 Python node that publishes multi-channel biopotential data from an LibIIO source',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'publish = ad4114exg_publisher.node:main'
        ],
    },
)
