from setuptools import setup, find_packages

setup(
	name="lumi-taskdev",
	description="Python bindings for taskdev library",
	author_email="ksyusha.churkina.2005@mail.ru",
	packages=find_packages(),
	cffi_modules=["build_taskdev.py:ffi"],
)	
