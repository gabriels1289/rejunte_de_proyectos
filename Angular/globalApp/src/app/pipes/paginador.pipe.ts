import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'paginador'
})
export class PaginadorPipe implements PipeTransform {

  // transform(array: any[], pageSize: number | string, pageNumber: number): any[] {
  /* transform(array: any[], pageSize: number, pageIndex: number): any[] {
    if (!array.length) {
      return [];
    }
    // if (pageSize === 'all') {
    //  return array;
    // }
    pageSize = pageSize || 10;
    pageIndex = pageIndex || 1;
    --pageIndex;
    return array.slice(pageIndex * pageSize, (pageIndex + 1) * pageSize);
  } */

  transform(array: any[], pageSize: number, pageIndex: number): any[] {
    if (!array.length) {
      return [];
    }
    const startIndex = pageIndex * pageSize;
    let endIndex = startIndex + pageSize;
    if (endIndex > array.length) {
      endIndex = array.length;
    }
    return array.slice(startIndex, endIndex);
  }

}
