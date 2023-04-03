import { NativeFile } from './file';
import { PointerByReference } from './pointer_by_reference';

export class NativeFileParser {
  wasmModule: any;
  ptr: number;
  dataPtr: number;

  constructor(wasmModule: any, data: Uint8Array) {
    this.wasmModule = wasmModule;
    const parserPtrRef = new PointerByReference(wasmModule);
    const dataPtr = wasmModule._malloc(data.byteLength);
    wasmModule.HEAPU8.set(data, dataPtr);
    const result = wasmModule.ccall('rgbd_record_parser_ctor_from_data', 'number', ['number', 'number', 'number'], [parserPtrRef.ptr, dataPtr, data.byteLength]);

    const parserPtr = parserPtrRef.getValue();
    parserPtrRef.close();

    if (result < 0)
      throw new Error('Failed to createFileParserFromData');

    this.ptr = parserPtr;
    // Need to keep this dataPtr alive to read the data using the parser.
    this.dataPtr = dataPtr;
  }

  close() {
    this.wasmModule.ccall('rgbd_record_parser_dtor', null, ['number'], [this.ptr]);
    this.wasmModule._free(this.dataPtr);
  }

  parse(withFrames: boolean, withDirections: boolean): NativeFile {
    const filePtr = this.wasmModule.ccall('rgbd_record_parser_parse',
                                          'number',
                                          ['number', 'boolean', 'boolean'],
                                          [this.ptr, withFrames, withDirections]);
    return new NativeFile(this.wasmModule, filePtr);
  }
}
