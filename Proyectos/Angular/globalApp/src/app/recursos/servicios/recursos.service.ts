import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { catchError } from 'rxjs/operators';

// esto es para intentar acceder a los empleados mediante la base de datos
import { HttpClient, HttpParams, HttpHeaders, HttpResponse } from '@angular/common/http';
import { HttpErrorHandlerService, handleError } from '../../_helpers/http-error-handler.service';

// importo la direccion de la api
import { environment } from '../../../environments/environment';  // TODO: cambiar a prod

@Injectable({
  providedIn: 'root'
})
export class RecursosService {

  private handleError: handleError;

  constructor(
  private http: HttpClient,
  httpErrorhandler: HttpErrorHandlerService
  ) {
    this.handleError = httpErrorhandler.createHandleError('RecursosService');
  }

  getBiblioteca(idsector: number) {
    const  params = new  HttpParams().set('idsector', idsector.toString());
    return this.http.get(`${environment.apiUrl}/index.php/recursos/biblioteca`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getBiblioteca', []))
      );
  }

  getListadoDeRecursos(idusuario: number) {
    const  params = new  HttpParams().set('idusuario', idusuario.toString());
    return this.http.get(`${environment.apiUrl}/index.php/recursos/gestor`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getListadoDeRecursos', []))
      );
  }

  putAltaBaja(data: any) {
    return this.http.put(`${environment.apiUrl}/index.php/recursos/gestor`, data, {observe: 'response'})
      .pipe(
        catchError(this.handleError('putAltaBaja', []))
      );
  }

  deleteRecurso(data: any) {
    return this.http.put(`${environment.apiUrl}/index.php/recursos/gestor`, data, {observe: 'response'})
      .pipe(
        catchError(this.handleError('deleteRecurso', []))
      );
  }

  postRecurso(formData: FormData) {
    return this.http.post<any>(`${environment.apiUrl}/index.php/recursos/gestor`, formData, {observe: 'response'})
      .pipe(
        catchError(this.handleError('postRecurso', []))
      );
  }
}
